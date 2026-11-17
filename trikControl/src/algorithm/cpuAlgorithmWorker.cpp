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
#include <memory>
#include <utility>

#include <QsLog.h>

using namespace trikControl;

CpuAlgorithmWorker::CpuAlgorithmWorker(QSharedPointer<CameraManager> camera, QString port,
	trikHal::HardwareAbstractionInterface &hal, std::shared_ptr<VideoPreviewSink> previewSink,
	int previewBufferCount, QObject *parent)
	: VideoAlgorithmWorker(std::move(camera), std::move(port), hal, std::move(previewSink), previewBufferCount,
		  parent)
{
}

CpuAlgorithmWorker::~CpuAlgorithmWorker() = default;

trikHal::FrameFormat CpuAlgorithmWorker::scaledFormat(int srcWidth, int srcHeight) const
{
	return previewFormat(srcWidth, srcHeight);
}

void CpuAlgorithmWorker::handleFrame(Frame &frame)
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

	if (previewOn()) {
		previewBuffer = acquirePreviewBuffer(dstFormat);
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
	if (previewBuffer) {
		overlay = buildOverlay(view);
	}

	dst->syncEnd(writeSync);

	if (previewBuffer) {
		publishPreview(std::move(previewBuffer), overlay);
	}
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

	const auto format = rgaFormat(srcFormat);
	if (format == trikHal::RgaFormat::Unknown) {
		if (!mRgaErrorLogged) {
			QLOG_ERROR() << "CpuAlgorithmWorker: RGA does not support the capture format on" << mPort;
			mRgaErrorLogged = true;
		}
		return false;
	}

	auto srcStridePixels = 0;
	if (!stageSource(src, static_cast<uint32_t>(srcStride), static_cast<uint32_t>(srcHeight), srcFormat,
		    &srcStridePixels)) {
		if (!mRgaErrorLogged) {
			QLOG_ERROR() << "CpuAlgorithmWorker: failed to stage the camera frame on" << mPort;
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

	const auto srcImage = trikHal::RgaImage {mScaleInput->fd(), srcWidth, srcHeight, format, srcStridePixels};
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
