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

#include <QtCore/QDebug>

#include <trikKernel/configurer.h>

#include "src/colorSensorWorker.h"

using namespace trikControl;

ColorSensor::ColorSensor(QString const &port, const trikKernel::Configurer &configurer)
{
	QString const &script = configurer.attributeByPort(port, "script");
	QString const &inputFile = configurer.attributeByPort(port, "inputFile");
	QString const &outputFile = configurer.attributeByPort(port, "outputFile");
	int const m = configurer.attributeByPort(port, "m").toInt();
	int const n = configurer.attributeByPort(port, "n").toInt();

	mColorSensorWorker.reset(new ColorSensorWorker(script, inputFile, outputFile, m, n));
	mColorSensorWorker->moveToThread(&mWorkerThread);

	connect(mColorSensorWorker.data(), SIGNAL(stopped()), this, SIGNAL(stopped()));

	mWorkerThread.start();
}

ColorSensor::~ColorSensor()
{
	mWorkerThread.quit();
	mWorkerThread.wait();
}

void ColorSensor::init(bool showOnDisplay)
{
	QMetaObject::invokeMethod(mColorSensorWorker.data(), "init", Q_ARG(bool, showOnDisplay));
}

QVector<int> ColorSensor::read(int m, int n)
{
	// Read is called synchronously and only takes prepared value from sensor.
	return mColorSensorWorker->read(m, n);
}

void ColorSensor::stop()
{
	QMetaObject::invokeMethod(mColorSensorWorker.data(), "stop");
}
