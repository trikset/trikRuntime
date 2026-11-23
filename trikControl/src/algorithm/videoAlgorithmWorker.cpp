/* Copyright 2026 CyberTech Labs Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#include "videoAlgorithmWorker.h"

#include <algorithm>
#include <cstring>
#include <utility>

#include <QsLog.h>

using namespace trikControl;

VideoAlgorithmWorker::VideoAlgorithmWorker(QSharedPointer<CameraManager> camera, QString port,
	trikHal::HardwareAbstractionInterface &hal, std::shared_ptr<VideoPreviewSink> previewSink,
	int previewBufferCount, QObject *parent)
	: QThread(parent)
	, mCamera(std::move(camera))
	, mPortId(mCamera ? mCamera->portId(port) : -1)
	, mPort(std::move(port))
	, mHal(hal)
	, mSink(std::move(previewSink))
	, mRga(mHal.createRga())
	, mDmaHeap(mHal.createDmaHeap())
	, mPreviewBufferCount(previewBufferCount)
{
	mAvailable = mRga && mRga->isAvailable() && mDmaHeap;
	if (!mAvailable) {
		QLOG_ERROR() << "VideoAlgorithmWorker: RGA is not available on" << mPort;
	}
}

VideoAlgorithmWorker::~VideoAlgorithmWorker()
{
	stop(StopAll);
}

bool VideoAlgorithmWorker::isReadyToStart() const
{
	return mAvailable;
}

void VideoAlgorithmWorker::start(bool showOnDisplay)
{
	if (!isReadyToStart() || !mCamera || isRunning()) {
		return;
	}

	if (mPortId >= 0 && !mCameraAcquired) {
		mCamera->acquire(mPortId);
		mCameraAcquired = true;
	}

	if (showOnDisplay && mSink && mPortId >= 0) {
		mPreviewId = mSink->openPreview();
		mPreviewEnabled = true;
	}

	mRunning.store(true, std::memory_order_release);
	mAbort.store(false, std::memory_order_release);
	QThread::start();
}

void VideoAlgorithmWorker::stop(int flags)
{
	const auto releaseCamera = (flags & (StopStream | StopAll)) != 0;

	mRunning.store(false, std::memory_order_release);
	mAbort.store(true, std::memory_order_release);

	if (mCamera) {
		mCamera->abortFrameWaits(&mAbort);
	}

	if (isRunning()) {
		wait();
	}

	if (mPreviewEnabled && mSink) {
		mSink->closePreview(mPreviewId);
		mPreviewId = -1;
		mPreviewEnabled = false;
	}

	if (releaseCamera && mCamera && mCameraAcquired) {
		// StopStream parks the device: stop streaming but keep it open for a
		// quick re-acquire. StopAll lets the last release close it.
		if (flags & StopStream) {
			mCamera->stopStreaming(mPort);
		}
		mCamera->release(mPortId);
		mCameraAcquired = false;
	}
}

void VideoAlgorithmWorker::run()
{
	if (!mCamera) {
		QLOG_ERROR() << "VideoAlgorithmWorker: no camera manager for" << mPort;
		return;
	}

	QLOG_INFO() << "VideoAlgorithmWorker: loop started on" << QThread::currentThread() << "for" << mPort;

	auto seenSeq = uint32_t {0};
	while (mRunning.load(std::memory_order_acquire)) {
		auto frame = mCamera->getFrame(mPortId, seenSeq, -1, &mAbort);
		if (!mRunning.load(std::memory_order_acquire)) {
			break;
		}
		if (!frame.isValid()) {
			continue;
		}

		seenSeq = frame.sequence();
		handleFrame(frame);
	}

	QLOG_INFO() << "VideoAlgorithmWorker: loop stopped for" << mPort;
}

trikHal::FrameFormat VideoAlgorithmWorker::previewFormat(int srcWidth, int srcHeight) const
{
	auto format = trikHal::FrameFormat {};
	format.width = static_cast<uint32_t>(std::min(srcWidth, static_cast<int>(trikKernel::videoPreviewWidth)));
	format.height = static_cast<uint32_t>(std::min(srcHeight, static_cast<int>(trikKernel::videoPreviewHeight)));
	format.stride = format.width * 4;
	format.format = trikHal::PixelFormat::Bgra8888;
	return format;
}

bool VideoAlgorithmWorker::stageSource(const Frame &frame, uint32_t srcStride, uint32_t srcHeight,
	trikKernel::PixelFormat srcFormat, int *srcStridePixels)
{
	const auto yuyv = srcFormat == trikKernel::PixelFormat::Yuyv;
	const auto stridePixels = static_cast<int>(yuyv ? srcStride / 2 : srcStride);
	const auto srcBytes =
		yuyv ? static_cast<size_t>(srcStride) * srcHeight : static_cast<size_t>(srcStride) * srcHeight * 2;

	if (!ensureInputBuffer(std::max(srcBytes, frame.size()))) {
		return false;
	}

	mScaleInput->syncStart(true);
	std::memcpy(mScaleInput->data(), frame.data(), std::min(frame.size(), mScaleInput->size()));
	mScaleInput->syncEnd(true);

	*srcStridePixels = stridePixels;
	return true;
}

bool VideoAlgorithmWorker::ensureInputBuffer(std::size_t bytes)
{
	if (mScaleInput && mScaleInput->size() >= bytes) {
		return true;
	}

	if (!mDmaHeap) {
		return false;
	}

	mScaleInput.reset(mDmaHeap->allocate(bytes));
	return mScaleInput != nullptr;
}

trikHal::RgaFormat VideoAlgorithmWorker::rgaFormat(trikKernel::PixelFormat format)
{
	switch (format) {
	case trikKernel::PixelFormat::Nv16:
		return trikHal::RgaFormat::Nv16;
	case trikKernel::PixelFormat::Yuyv:
		return trikHal::RgaFormat::Yuyv;
	default:
		return trikHal::RgaFormat::Unknown;
	}
}

bool VideoAlgorithmWorker::previewOn() const
{
	return mPreviewEnabled && mSink;
}

trikHal::FrameBufferPtr VideoAlgorithmWorker::acquirePreviewBuffer(const trikHal::FrameFormat &format)
{
	if (!previewOn()) {
		return nullptr;
	}

	return mHal.acquireFrameBuffer(format, mPreviewBufferCount);
}

trikHal::FrameBufferPtr VideoAlgorithmWorker::acquirePreviewBufferFor(int srcWidth, int srcHeight)
{
	return acquirePreviewBuffer(previewFormat(srcWidth, srcHeight));
}

void VideoAlgorithmWorker::publishPreview(trikHal::FrameBufferPtr buffer, const OverlayResult &overlay)
{
	if (!previewOn() || !buffer) {
		return;
	}

	auto preview = VideoPreviewFrame {};
	preview.buffer = std::move(buffer);
	preview.overlay = overlay.overlay;
	preview.overlayGeneration = overlay.generation;
	mSink->publish(mPreviewId, preview);
}
