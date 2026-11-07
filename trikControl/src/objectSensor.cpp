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

#include "objectSensor.h"

#include <QsLog.h>

#include <trikKernel/configurer.h>

#include <trikHal/hardwareAbstractionInterface.h>

#include "algorithm/objectSensorWorker.h"
#include "cameraManager.h"

using namespace trikControl;

ObjectSensor::ObjectSensor(const QString &port, const trikKernel::Configurer &configurer,
	const QSharedPointer<CameraManager> &cameraManager, trikHal::HardwareAbstractionInterface &hardware,
	const std::shared_ptr<VideoPreviewSink> &previewSink)
	: mState("Object Sensor on " + port)
	, mPort(port)
{
	auto defaultTolerance = QStringLiteral("1.0");
	auto toleranceOk = false;
	const auto tolerance =
		configurer.childAttributeByPort(port, "objectSensor", "toleranceFactor", &defaultTolerance)
			.toDouble(&toleranceOk);
	if (!toleranceOk) {
		QLOG_ERROR() << "ObjectSensor: invalid toleranceFactor on" << port;
		mState.fail();
		return;
	}

	const auto portId = cameraManager ? cameraManager->portId(port) : -1;
	if (portId < 0) {
		QLOG_ERROR() << "ObjectSensor: port" << port << "is not a configured videoDevice";
		mState.fail();
		return;
	}

	mWorker.reset(new ObjectSensorWorker(cameraManager, port, hardware, previewSink));
	mWorker->setToleranceFactor(tolerance);
	mState.ready();
}

ObjectSensor::~ObjectSensor()
{
	if (mWorker) {
		mWorker->stop(StopAll);
	}
}

ObjectSensor::Status ObjectSensor::status() const
{
	return mState.status();
}

void ObjectSensor::init(bool showOnDisplay)
{
	if (mState.isFailed() || !mWorker || mWorker->isRunning()) {
		return;
	}

	// The sensor is ready right after construction; a re-init after stop()
	// brings it back from off.
	if (!mState.isReady()) {
		mState.ready();
	}

	mWorker->start(showOnDisplay);
	QLOG_INFO() << "ObjectSensor: started on" << mPort;
}

void ObjectSensor::detect()
{
	if (mState.isReady() && mWorker) {
		mWorker->detect();
	}
}

QVector<int> ObjectSensor::read()
{
	return mWorker ? mWorker->reading() : QVector<int> {0, 0, 0};
}

QVector<int> ObjectSensor::getDetectParameters() const
{
	return mWorker ? mWorker->detectParameters() : QVector<int> {0, 0, 0, 0, 0, 0};
}

// NOLINTNEXTLINE(google-default-arguments)
void ObjectSensor::stop(int flags)
{
	const auto releaseCamera = (flags & (StopStream | StopAll)) != 0;

	if (mWorker) {
		mWorker->stop(flags);
	}

	if (releaseCamera && mState.isReady()) {
		mState.stop();
		mState.off();
	}

	Q_EMIT stopped();
}
