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

#include "npuAlgorithmWorker.h"

#include <utility>

#include <QsLog.h>

using namespace trikControl;

namespace {

/// BGRA bytes per pixel, used to convert the preview line stride to pixels.
constexpr int previewBgraBytesPerPixel = 4;

} // namespace

NpuAlgorithmWorker::NpuAlgorithmWorker(QSharedPointer<CameraManager> camera, QString port,
	trikHal::HardwareAbstractionInterface &hal, std::shared_ptr<VideoPreviewSink> previewSink,
	int previewBufferCount, QObject *parent)
	: VideoAlgorithmWorker(std::move(camera), std::move(port), hal, std::move(previewSink), previewBufferCount,
		  parent)
{
	mSession.reset(mHal.createRknnSession());
	if (!mSession) {
		QLOG_ERROR() << "NpuAlgorithmWorker: NPU is not available on" << mPort;
	}
}

bool NpuAlgorithmWorker::loadModel(const QString &modelPath, QString *error)
{
	if (!mSession) {
		if (error) {
			*error = QStringLiteral("NPU is not available");
		}
		return false;
	}

	if (!mSession->load(modelPath, error)) {
		mLoaded = false;
		return false;
	}

	mInputInfo = mSession->input();
	mLoaded = true;
	return true;
}

const trikHal::RknnTensorInfo &NpuAlgorithmWorker::modelInput() const
{
	return mInputInfo;
}

bool NpuAlgorithmWorker::isReadyToStart() const
{
	return mAvailable && mLoaded && mSession;
}

OverlayResult NpuAlgorithmWorker::buildOverlay(const QSize &previewSize)
{
	Q_UNUSED(previewSize)
	return OverlayResult {};
}

void NpuAlgorithmWorker::handleFrame(Frame &frame)
{
	const auto info = mCamera->info(mPortId);
	auto srcWidth = uint32_t {0};
	auto srcHeight = uint32_t {0};
	auto srcStride = uint32_t {0};
	if (!frame.data() || !info.captureGeometry(frame.size(), &srcWidth, &srcHeight, &srcStride)) {
		if (!mGeometryWarned) {
			QLOG_ERROR() << "NpuAlgorithmWorker: invalid capture frame on" << mPort << "(" << frame.size()
				     << "bytes)";
			mGeometryWarned = true;
		}
		frame.release();
		return;
	}

	// The target geometry comes from the model, never hardcoded here.
	auto modelWidth = 0;
	auto modelHeight = 0;
	const auto &dims = mInputInfo.dims;
	if (dims.size() >= 4) {
		if (mInputInfo.layout == trikHal::RknnLayout::Nhwc) {
			modelHeight = static_cast<int>(dims[1]);
			modelWidth = static_cast<int>(dims[2]);
		} else {
			modelHeight = static_cast<int>(dims[2]);
			modelWidth = static_cast<int>(dims[3]);
		}
	}

	const auto dstFd = mSession->inputBufferFd();
	if (modelWidth <= 0 || modelHeight <= 0 || dstFd < 0) {
		if (!mGeometryWarned) {
			QLOG_ERROR() << "NpuAlgorithmWorker: model input is not usable on" << mPort;
			mGeometryWarned = true;
		}
		frame.release();
		return;
	}

	const auto format = rgaFormat(info.format);
	if (format == trikHal::RgaFormat::Unknown) {
		if (!mRgaErrorLogged) {
			QLOG_ERROR() << "NpuAlgorithmWorker: RGA does not support the capture format on" << mPort;
			mRgaErrorLogged = true;
		}
		frame.release();
		return;
	}

	auto srcStridePixels = 0;
	if (!stageSource(frame, srcStride, srcHeight, info.format, &srcStridePixels)) {
		if (!mRgaErrorLogged) {
			QLOG_ERROR() << "NpuAlgorithmWorker: failed to stage the camera frame on" << mPort;
			mRgaErrorLogged = true;
		}
		frame.release();
		return;
	}

	frame.release();

	const auto dstStride = mInputInfo.wStride > 0 ? static_cast<int>(mInputInfo.wStride) : modelWidth;
	const auto srcImage = trikHal::RgaImage {mScaleInput->fd(), static_cast<int>(srcWidth),
		static_cast<int>(srcHeight), format, srcStridePixels};
	const auto dstImage = trikHal::RgaImage {dstFd, modelWidth, modelHeight, trikHal::RgaFormat::Rgb888, dstStride};

	auto error = QString {};
	if (!mRga->process(srcImage, dstImage, &error)) {
		if (!mRgaErrorLogged) {
			QLOG_ERROR() << "NpuAlgorithmWorker: RGA process failed on" << mPort << ":" << error;
			mRgaErrorLogged = true;
		}
		return;
	}

	// Optional display pass: RGA again, this time into a BGRA preview buffer.
	// The NPU input is RGB888, so the same buffer cannot be shown on screen.
	if (previewOn()) {
		auto previewBuffer = acquirePreviewBufferFor(static_cast<int>(srcWidth), static_cast<int>(srcHeight));
		if (previewBuffer) {
			const auto &previewFormat = previewBuffer->format();
			const auto previewImage =
				trikHal::RgaImage {previewBuffer->fd(), static_cast<int>(previewFormat.width),
					static_cast<int>(previewFormat.height), trikHal::RgaFormat::Bgra8888,
					static_cast<int>(previewFormat.stride / previewBgraBytesPerPixel)};
			auto previewError = QString {};
			if (mRga->process(srcImage, previewImage, &previewError)) {
				const auto previewSize = QSize(static_cast<int>(previewFormat.width),
					static_cast<int>(previewFormat.height));
				publishPreview(std::move(previewBuffer), buildOverlay(previewSize));
			} else {
				QLOG_WARN()
					<< "NpuAlgorithmWorker: RGA preview failed on" << mPort << ":" << previewError;
			}
		}
	}

	auto outputs = QVector<trikHal::RknnOutputView> {};
	if (!mSession->run(&outputs, &error)) {
		QLOG_WARN() << "NpuAlgorithmWorker: inference failed on" << mPort << ":" << error;
		return;
	}

	processOutputs(outputs);
}

trikHal::RknnSessionInterface &NpuAlgorithmWorker::session()
{
	return *mSession;
}

const QString &NpuAlgorithmWorker::port() const
{
	return mPort;
}
