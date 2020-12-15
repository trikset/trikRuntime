/* Copyright 2014 - 2015 CyberTech Labs Ltd.
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

#include <trikKernel/configurer.h>
#include <QsLog.h>

#include "objectSensorWorker.h"
#include "configurerHelper.h"

using namespace trikControl;

ObjectSensor::ObjectSensor(const QString &port, const trikKernel::Configurer &configurer
		, trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mState("Object Sensor on" + port)
{
	const QString &script = configurer.attributeByPort(port, "script");
	const QString &inputFile = configurer.attributeByPort(port, "inputFile");
	const QString &outputFile = configurer.attributeByPort(port, "outputFile");
	const qreal toleranceFactor = ConfigurerHelper::configureReal(configurer, mState, port, "toleranceFactor");

	if (!mState.isFailed()) {
		mObjectSensorWorker.reset(new ObjectSensorWorker(script, inputFile, outputFile, toleranceFactor, mState
				, hardwareAbstraction));
		mObjectSensorWorker->moveToThread(&mWorkerThread);
		connect(mObjectSensorWorker.data(), SIGNAL(stopped()), this, SLOT(onStopped()), Qt::DirectConnection);

		QLOG_INFO() << "Starting ObjectSensor worker thread" << &mWorkerThread;

		mWorkerThread.setObjectName(mObjectSensorWorker->metaObject()->className());
		mWorkerThread.start();
	}
}

ObjectSensor::~ObjectSensor()
{
	if (mWorkerThread.isRunning()) {
		mWorkerThread.quit();
		mWorkerThread.wait();
	}
}

ObjectSensor::Status ObjectSensor::status() const
{
	return mState.status();
}

void ObjectSensor::init(bool showOnDisplay)
{
	if (!mState.isFailed()) {
		QMetaObject::invokeMethod(mObjectSensorWorker.data(), "init", Q_ARG(bool, showOnDisplay));
	}
}

void ObjectSensor::detect()
{
	if (mState.isReady()) {
		QMetaObject::invokeMethod(mObjectSensorWorker.data(), "detect");
	} else {
		QLOG_ERROR() << "Trying to call 'detect' when sensor is not ready, ignoring";
	}
}

QVector<int>  ObjectSensor::read()
{
	if (mState.isReady()) {
		// Read is called synchronously and only takes prepared value from sensor.
		return mObjectSensorWorker->read();
	} else {
		QLOG_ERROR() << "Trying to call 'read' when sensor is not ready, ignoring";
		return {};
	}
}

void ObjectSensor::stop()
{
	if (mState.isReady()) {
		/// @todo Correctly stop starting sensors.
		QMetaObject::invokeMethod(mObjectSensorWorker.data(), "stop");
	}
}

QVector<int> ObjectSensor::getDetectParameters() const
{
	if (mState.isReady()) {
		// Read is called synchronously and only takes prepared value from sensor.
		return mObjectSensorWorker->getDetectParameters();
	} else {
		QLOG_ERROR() << "Trying to call 'read' when sensor is not ready, ignoring";
		return {};
	}
}

void ObjectSensor::onStopped()
{
	emit stopped();
}
