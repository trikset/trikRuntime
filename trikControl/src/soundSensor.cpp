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

#include "soundSensor.h"

#include <trikKernel/configurer.h>
#include <QsLog.h>

#include "soundSensorWorker.h"
#include "configurerHelper.h"

using namespace trikControl;

SoundSensor::SoundSensor(QString const &port, trikKernel::Configurer const &configurer
		, trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mState("Sound Sensor on " + port)
{
	const QString script = configurer.attributeByPort(port, "script");
	const QString inputFile = configurer.attributeByPort(port, "inputFile");
	const QString outputFile = configurer.attributeByPort(port, "outputFile");

	if (!mState.isFailed()) {
		mSoundSensorWorker.reset(new SoundSensorWorker(script, inputFile, outputFile, mState, hardwareAbstraction));

		mSoundSensorWorker->moveToThread(&mWorkerThread);

		connect(mSoundSensorWorker.data(), SIGNAL(stopped()), this, SIGNAL(stopped()));

		mWorkerThread.start();
	}
}

SoundSensor::~SoundSensor()
{
	if (mWorkerThread.isRunning()) {
		mWorkerThread.quit();
		mWorkerThread.wait();
	}
}

SoundSensor::Status SoundSensor::status() const
{
	return mState.status();
}

void SoundSensor::init(bool showOnDisplay)
{
	if (!mState.isFailed()) {
		QMetaObject::invokeMethod(mSoundSensorWorker.data(), "init", Q_ARG(bool, showOnDisplay));
	}
}

void SoundSensor::detect()
{
	if (mState.isReady()) {
		QMetaObject::invokeMethod(mSoundSensorWorker.data(), "detect");
	}
	else
	{
		QLOG_ERROR() << "Trying to call 'detect' when sensor is not ready, ignoring";
	}
}

void SoundSensor::volume(int volCoeff)
{
	if (mState.isReady()) {
		QMetaObject::invokeMethod(mSoundSensorWorker.data(), "volume", Q_ARG(int, volCoeff));
	}
	else
	{
		QLOG_ERROR() << "Trying to call 'volume' when sensor is not ready, ignoring";
	}
}

QVector<int>  SoundSensor::read()
{
	if (mState.isReady()) {
		// Read is called synchronously and only takes prepared value from sensor.
		return mSoundSensorWorker->read();
	} else {
		QLOG_ERROR() << "Trying to call 'read' when sensor is not ready, ignoring";
		return {};
	}
}

void SoundSensor::stop()
{
	if (mState.isReady()) {
		QMetaObject::invokeMethod(mSoundSensorWorker.data(), "stop");
	} else {
		QLOG_ERROR() << "Trying to call 'stop' when sensor is not ready, ignoring";
	}
}
