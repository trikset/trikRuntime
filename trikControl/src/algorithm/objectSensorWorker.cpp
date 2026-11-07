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

#include <utility>

#include <QtCore/QMutexLocker>

using namespace trikControl;

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

	mDetector.process(view);

	{
		QMutexLocker locker(&mResultLock);
		mResult = mDetector.result();
	}

	return true;
}

OverlayResult ObjectSensorWorker::buildOverlay(const BgraFrameView &frame)
{
	Q_UNUSED(frame);
	return {};
}
