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

#include "colorSensor.h"

#include <trikKernel/configurer.h>
#include <QsLog.h>

#include "colorSensorWorker.h"
#include "configurerHelper.h"

using namespace trikControl;

ColorSensor::ColorSensor(const QString &port, const trikKernel::Configurer &configurer
		, trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mState("Color Sensor on" + port)
{
	const QString &script = configurer.attributeByPort(port, "script");
	const QString &inputFile = configurer.attributeByPort(port, "inputFile");
	const QString &outputFile = configurer.attributeByPort(port, "outputFile");

	const int m = ConfigurerHelper::configureInt(configurer, mState, port, "m");
	const int n = ConfigurerHelper::configureInt(configurer, mState, port, "n");

	mColorSensorWorker.reset(new ColorSensorWorker(script, inputFile, outputFile, m, n, mState, hardwareAbstraction));
	mColorSensorWorker->moveToThread(&mWorkerThread);

	connect(mColorSensorWorker.data(), &ColorSensorWorker::stopped
			, this, &ColorSensor::onStopped, Qt::DirectConnection);

	QLOG_INFO() << "Starting ColorSensor worker thread" << &mWorkerThread;

	mWorkerThread.setObjectName(mColorSensorWorker->metaObject()->className());
	mWorkerThread.start();
}

ColorSensor::~ColorSensor()
{
	if (mWorkerThread.isRunning()) {
		mWorkerThread.quit();
		mWorkerThread.wait();
	}
}

ColorSensor::Status ColorSensor::status() const
{
	return mColorSensorWorker->status();
}

void ColorSensor::init(bool showOnDisplay)
{
	QMetaObject::invokeMethod(mColorSensorWorker.data()
							  , [this, showOnDisplay](){mColorSensorWorker->init(showOnDisplay);});
}

QVector<int> ColorSensor::read(int m, int n)
{
	// Read is called synchronously and only takes prepared value from sensor.
	return mColorSensorWorker->read(m, n);
}

void ColorSensor::stop()
{
	QMetaObject::invokeMethod(mColorSensorWorker.data(), &ColorSensorWorker::stop);
}

void ColorSensor::onStopped()
{
	emit stopped();
}
