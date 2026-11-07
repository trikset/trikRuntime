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

#include "objectSensorWorker.h"

#include <algorithm>
#include <utility>

#include <QtCore/QMutexLocker>

#include "frameDrawingUtils.h"

using namespace trikControl;

namespace {

/// Cyan highlight painted over the detected clusters (BGRA).
constexpr auto clusterColor = uint32_t {0xFF00FFFF};
/// Magenta guide lines that mark the detection zone boundaries (BGRA).
constexpr auto guideColor = uint32_t {0xFFFF00FF};
/// Red target markers (BGRA).
constexpr auto targetColor = uint32_t {0xFFFF0000};

/// Half-length of each guide line around the frame center, in pixels.
constexpr auto guideHalfLength = 99;
/// Zone scale used by ObjectDetector::detect(): step = frame.height / 6.
constexpr auto detectZoneScale = 6;
/// Number of guide line pairs drawn on each side of the center.
constexpr auto guidePairs = 2;

} // namespace

ObjectSensorWorker::ObjectSensorWorker(QSharedPointer<CameraManager> camera, QString port,
	trikHal::HardwareAbstractionInterface &hal, std::shared_ptr<VideoPreviewSink> previewSink,
	int previewBufferCount, QObject *parent)
	: CpuAlgorithmWorker(std::move(camera), std::move(port), hal, std::move(previewSink), previewBufferCount,
		  parent)
{
}

void ObjectSensorWorker::detect()
{
	mDetectRequested.store(true, std::memory_order_release);
}

QVector<int> ObjectSensorWorker::reading() const
{
	QMutexLocker locker(&mResultLock);
	return {mResult.x, mResult.y, mResult.size};
}

QVector<int> ObjectSensorWorker::detectParameters() const
{
	QMutexLocker locker(&mDetectParametersLock);
	return {mDetectParameters.hue, mDetectParameters.saturation, mDetectParameters.value,
		mDetectParameters.hueTolerance, mDetectParameters.saturationTolerance,
		mDetectParameters.valueTolerance};
}

void ObjectSensorWorker::setToleranceFactor(qreal factor)
{
	mToleranceFactor = factor;
}

bool ObjectSensorWorker::processFrame(const BgraFrameView &frame)
{
	if (!frame.data || frame.width <= 0 || frame.height <= 0 || frame.stride < frame.width * bgraBytesPerPixel) {
		return false;
	}

	const auto view = ObjectDetector::FrameView {frame.data, frame.width, frame.height, frame.stride};

	if (mDetectRequested.exchange(false, std::memory_order_acq_rel)) {
		const auto detected = mDetector.detect(view, static_cast<double>(mToleranceFactor));

		auto parameters = DetectParameters {};

		// Convert the detector's 8-bit HSV window to the legacy 0..359 / 0..100
		// representation. Value is kept at 50% with full tolerance.
		if (detected.valid) {
			const auto &ranges = detected.ranges;
			const auto hueFrom = static_cast<int>(ranges.hue.from);
			const auto hueTo = static_cast<int>(ranges.hue.to);

			auto hueCenter = 0;
			auto hueTolerance = 0;
			if (ranges.hueWraps) {
				const auto width = (hueTo - hueFrom + 256) % 256;
				hueCenter = (hueFrom + width / 2) % 256;
				hueTolerance = width / 2;
			} else {
				hueCenter = (hueFrom + hueTo) / 2;
				hueTolerance = (hueTo - hueFrom) / 2;
			}

			const auto saturationCenter =
				(static_cast<int>(ranges.saturation.from) + static_cast<int>(ranges.saturation.to)) / 2;
			const auto saturationTolerance =
				(static_cast<int>(ranges.saturation.to) - static_cast<int>(ranges.saturation.from)) / 2;

			parameters = {hueCenter * 359 / 255, saturationCenter * 100 / 255, 50, hueTolerance * 359 / 255,
				saturationTolerance * 100 / 255, 50};
		}

		{
			QMutexLocker locker(&mDetectParametersLock);
			mDetectParameters = parameters;
		}
	}

	// Highlights the detected metapixels while building the bitmap and fills
	// the result that is exposed through reading().
	mDetector.process(view, frame.data, clusterColor);
	const auto &result = mDetector.result();

	{
		QMutexLocker locker(&mResultLock);
		mResult = result;
	}

	drawTargetGuides(frame);

	// Red 3x3 marker at the center of every detected target.
	for (auto i = 0; i < result.targetCount; ++i) {
		const auto &target = result.targets[i];
		for (auto dy = -1; dy <= 1; ++dy) {
			drawHorizontalLine(frame, target.y + dy, target.x - 1, target.x + 1, targetColor);
		}
	}

	return true;
}

void ObjectSensorWorker::drawTargetGuides(const BgraFrameView &frame) const
{
	const auto centerX = frame.width / 2;
	const auto centerY = frame.height / 2;
	const auto step = std::max(1, frame.height / detectZoneScale);

	const auto top = centerY - guideHalfLength;
	const auto bottom = centerY + guideHalfLength;
	const auto left = centerX - guideHalfLength;
	const auto right = centerX + guideHalfLength;

	// Pairs of lines at one and two steps from the center mark the positive
	// and negative detection zones.
	for (auto i = 1; i <= guidePairs; ++i) {
		drawVerticalLine(frame, centerX - i * step, top, bottom, guideColor);
		drawVerticalLine(frame, centerX + i * step, top, bottom, guideColor);
		drawHorizontalLine(frame, centerY - i * step, left, right, guideColor);
		drawHorizontalLine(frame, centerY + i * step, left, right, guideColor);
	}
}

OverlayResult ObjectSensorWorker::buildOverlay(const BgraFrameView &frame)
{
	Q_UNUSED(frame);
	return {};
}
