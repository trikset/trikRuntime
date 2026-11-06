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
	QMutexLocker locker(&mReadingLock);
	return {mReading.x, mReading.y, mReading.size};
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
	Q_UNUSED(frame);
	mDetectRequested.store(false, std::memory_order_release);
	return true;
}

OverlayResult ObjectSensorWorker::buildOverlay(const BgraFrameView &frame)
{
	Q_UNUSED(frame);
	return {};
}
