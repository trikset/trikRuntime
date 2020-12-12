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

#include "lineSensor.h"

#include <trikKernel/configurer.h>
#include <QsLog.h>

#include "lineSensorWorker.h"
#include "configurerHelper.h"

using namespace trikControl;

LineSensor::LineSensor(const QString &port, const trikKernel::Configurer &configurer
		, trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mState("Line Sensor on " + port)
{
	const QString &script = configurer.attributeByPort(port, "script");
	const QString &inputFile = configurer.attributeByPort(port, "inputFile");
	const QString &outputFile = configurer.attributeByPort(port, "outputFile");
	const qreal toleranceFactor = ConfigurerHelper::configureReal(configurer, mState, port, "toleranceFactor");

	if (!mState.isFailed()) {
		mLineSensorWorker.reset(new LineSensorWorker(script, inputFile, outputFile, toleranceFactor, mState
				, hardwareAbstraction));

		mLineSensorWorker->moveToThread(&mWorkerThread);
		connect(mLineSensorWorker.data(), &LineSensorWorker::stopped, this, &LineSensor::onStopped);

		QLOG_INFO() << "Starting LineSensor worker thread" << &mWorkerThread;

		mWorkerThread.setObjectName(mLineSensorWorker->metaObject()->className());
		mWorkerThread.start();
	}
}

LineSensor::~LineSensor()
{
	if (mWorkerThread.isRunning()) {
		mWorkerThread.quit();
		mWorkerThread.wait();
	}
}

LineSensor::Status LineSensor::status() const
{
	return mState.status();
}

void LineSensor::init(bool showOnDisplay)
{
	if (!mState.isFailed()) {
		QMetaObject::invokeMethod(mLineSensorWorker.data()
								  , [this, showOnDisplay](){mLineSensorWorker->init(showOnDisplay);});
	}
}

void LineSensor::detect()
{
	if (mState.isReady()) {
		QMetaObject::invokeMethod(mLineSensorWorker.data(), &LineSensorWorker::detect);
	} else {
		QLOG_WARN() << "Calling 'detect' for sensor which is not ready";
	}
}

QVector<int> LineSensor::read()
{
	if (mState.isReady()) {
		// Read is called synchronously and only takes prepared value from sensor.
		return mLineSensorWorker->read();
	} else {
		QLOG_WARN() << "Calling 'read' for sensor which is not ready";
		return {};
	}
}

void LineSensor::stop()
{
	if (mState.isReady()) {
		/// @todo Correctly stop starting sensor.
		QMetaObject::invokeMethod(mLineSensorWorker.data(), &LineSensorWorker::stop);
	}
}

QVector<int> LineSensor::getDetectParameters() const
{
	if (mState.isReady()) {
		// Read is called synchronously and only takes prepared value from sensor.
		return mLineSensorWorker->getDetectParameters();
	} else {
		QLOG_WARN() << "Calling 'read' for sensor which is not ready";
		return {};
	}
}

void LineSensor::onStopped()
{
	emit stopped();
}
