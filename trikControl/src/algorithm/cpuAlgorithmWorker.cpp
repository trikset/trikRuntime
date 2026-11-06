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

#include "cpuAlgorithmWorker.h"

#include <algorithm>
#include <cstring>
#include <memory>
#include <utility>

#include <QsLog.h>

using namespace trikControl;

namespace {

trikHal::RgaFormat toRgaFormat(trikKernel::PixelFormat format)
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

} // namespace

trikHal::FrameFormat CpuAlgorithmWorker::scaledFormat(int srcWidth, int srcHeight) const
{
	auto format = trikHal::FrameFormat {};
	format.width = static_cast<uint32_t>(std::min(srcWidth, static_cast<int>(trikKernel::videoPreviewWidth)));
	format.height = static_cast<uint32_t>(std::min(srcHeight, static_cast<int>(trikKernel::videoPreviewHeight)));
	format.stride = format.width * bgraBytesPerPixel;
	format.format = trikHal::PixelFormat::Bgra8888;
	return format;
}

CpuAlgorithmWorker::CpuAlgorithmWorker(QSharedPointer<CameraManager> camera, QString port,
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
		QLOG_ERROR() << "CpuAlgorithmWorker: RGA is not available on" << mPort;
	}
}

CpuAlgorithmWorker::~CpuAlgorithmWorker()
{
	stop(StopAll);
}

void CpuAlgorithmWorker::start(bool showOnDisplay)
{
	if (!mAvailable || !mCamera || isRunning()) {
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

void CpuAlgorithmWorker::stop(int flags)
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

void CpuAlgorithmWorker::run()
{
	if (!mCamera) {
		QLOG_ERROR() << "CpuAlgorithmWorker: no camera manager for" << mPort;
		return;
	}

	QLOG_INFO() << "CpuAlgorithmWorker: loop started on" << QThread::currentThread() << "for" << mPort;

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
		processSourceFrame(frame);
	}

	QLOG_INFO() << "CpuAlgorithmWorker: loop stopped for" << mPort;
}

void CpuAlgorithmWorker::processSourceFrame(Frame &frame)
{
	const auto info = mCamera->info(mPortId);
	auto srcWidth = uint32_t {0};
	auto srcHeight = uint32_t {0};
	auto srcStride = uint32_t {0};
	if (!frame.data() || !info.captureGeometry(frame.size(), &srcWidth, &srcHeight, &srcStride)) {
		if (!mGeometryWarned) {
			QLOG_ERROR() << "CpuAlgorithmWorker: invalid capture frame on" << mPort << "(" << frame.size()
				     << "bytes)";
			mGeometryWarned = true;
		}
		frame.release();
		return;
	}

	const auto dstFormat = scaledFormat(static_cast<int>(srcWidth), static_cast<int>(srcHeight));
	if (dstFormat.width == 0 || dstFormat.height == 0 || dstFormat.stride == 0) {
		frame.release();
		return;
	}

	// Prefer a pooled buffer from HAL so the same dma-buf can go to the GPU.
	// If every buffer is held by a consumer, fall back to a private scratch
	// buffer and skip publishing: the algorithm must keep working regardless
	// of display load.
	auto previewBuffer = trikHal::FrameBufferPtr {};
	trikHal::FrameBuffer *dst = nullptr;

	if (mPreviewEnabled && mSink) {
		previewBuffer = mHal.acquireFrameBuffer(dstFormat, mPreviewBufferCount);
		dst = previewBuffer.get();
	}

	if (!dst) {
		if (!ensureScratch(dstFormat)) {
			frame.release();
			return;
		}
		dst = mScratch.get();
	}

	auto view = BgraFrameView {};
	if (!scaleWithRga(frame, static_cast<int>(srcWidth), static_cast<int>(srcHeight), static_cast<int>(srcStride),
		    info.format, dstFormat, *dst, &view)) {
		frame.release();
		return;
	}

	frame.release();

	const auto writeSync = modifiesOutputFrame();

	if (!processFrame(view)) {
		dst->syncEnd(writeSync);
		return;
	}

	auto overlay = OverlayResult {};
	if (previewBuffer && mSink) {
		overlay = buildOverlay(view);
	}

	dst->syncEnd(writeSync);

	if (previewBuffer && mSink) {
		auto preview = VideoPreviewFrame {};
		preview.buffer = previewBuffer;
		preview.overlay = overlay.overlay;
		preview.overlayGeneration = overlay.generation;
		mSink->publish(mPreviewId, preview);
	}
}

bool CpuAlgorithmWorker::ensureInputBuffer(size_t bytes)
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

bool CpuAlgorithmWorker::ensureScratch(const trikHal::FrameFormat &format)
{
	if (!mDmaHeap) {
		return false;
	}

	if (mScratch && mScratch->size() >= format.byteSize()) {
		mScratch->setFormat(format);
		return true;
	}

	auto dma = std::unique_ptr<trikHal::DmaBufferInterface>(mDmaHeap->allocate(format.byteSize()));
	if (!dma) {
		return false;
	}

	mScratch = std::make_unique<trikHal::FrameBuffer>(std::move(dma), format);
	return true;
}

bool CpuAlgorithmWorker::scaleWithRga(const Frame &src, int srcWidth, int srcHeight, int srcStride,
	trikKernel::PixelFormat srcFormat, const trikHal::FrameFormat &dstFormat, trikHal::FrameBuffer &dst,
	BgraFrameView *out)
{
	if (!mRga || !mDmaHeap) {
		return false;
	}

	const auto rgaFormat = toRgaFormat(srcFormat);
	if (rgaFormat == trikHal::RgaFormat::Unknown) {
		if (!mRgaErrorLogged) {
			QLOG_ERROR() << "CpuAlgorithmWorker: RGA does not support the capture format on" << mPort;
			mRgaErrorLogged = true;
		}
		return false;
	}

	const auto yuyv = srcFormat == trikKernel::PixelFormat::Yuyv;
	const auto srcStridePixels = static_cast<int>(yuyv ? srcStride / 2 : srcStride);
	const auto srcBytes =
		yuyv ? static_cast<size_t>(srcStride) * srcHeight : static_cast<size_t>(srcStride) * srcHeight * 2;

	if (!ensureInputBuffer(std::max(srcBytes, src.size()))) {
		if (!mRgaErrorLogged) {
			QLOG_ERROR() << "CpuAlgorithmWorker: failed to allocate RGA input on" << mPort;
			mRgaErrorLogged = true;
		}
		return false;
	}

	if (dst.size() < dstFormat.byteSize()) {
		if (!mRgaErrorLogged) {
			QLOG_ERROR() << "CpuAlgorithmWorker: RGA output buffer too small on" << mPort << "("
				     << dst.size() << "<" << dstFormat.byteSize() << ")";
			mRgaErrorLogged = true;
		}
		return false;
	}

	mScaleInput->syncStart(true);
	std::memcpy(mScaleInput->data(), src.data(), std::min(src.size(), mScaleInput->size()));
	mScaleInput->syncEnd(true);

	const auto srcImage = trikHal::RgaImage {mScaleInput->fd(), srcWidth, srcHeight, rgaFormat, srcStridePixels};
	const auto dstImage =
		trikHal::RgaImage {dst.fd(), static_cast<int>(dstFormat.width), static_cast<int>(dstFormat.height),
			trikHal::RgaFormat::Bgra8888, static_cast<int>(dstFormat.stride / bgraBytesPerPixel)};

	auto error = QString {};
	if (!mRga->process(srcImage, dstImage, &error)) {
		if (!mRgaErrorLogged) {
			QLOG_ERROR() << "CpuAlgorithmWorker: RGA process failed on" << mPort << ":" << error;
			mRgaErrorLogged = true;
		}
		return false;
	}

	dst.syncStart(false);

	out->data = static_cast<uint8_t *>(dst.data());
	out->width = static_cast<int>(dstFormat.width);
	out->height = static_cast<int>(dstFormat.height);
	out->stride = static_cast<int>(dstFormat.stride);
	return true;
}
