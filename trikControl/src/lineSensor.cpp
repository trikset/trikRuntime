/* Copyright 2014 CyberTech Labs Ltd.
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

#include "src/lineSensorWorker.h"

#include <QtCore/QDebug>

using namespace trikControl;

LineSensor::LineSensor(QString const &script, QString const &inputFile, QString const &outputFile
		, double toleranceFactor)
	: mLineSensorWorker(new LineSensorWorker(script, inputFile, outputFile, toleranceFactor))
{
	mLineSensorWorker->moveToThread(&mWorkerThread);
	mWorkerThread.start();
}

LineSensor::~LineSensor()
{
	mWorkerThread.quit();
	mWorkerThread.wait();
}

void LineSensor::init(bool showOnDisplay)
{
	QMetaObject::invokeMethod(mLineSensorWorker.data(), "init", Q_ARG(bool, showOnDisplay));
}

void LineSensor::detect()
{
	QMetaObject::invokeMethod(mLineSensorWorker.data(), "detect");
}

QVector<int> LineSensor::read()
{
	// Read is called synchronously and only takes prepared value from sensor.
	return mLineSensorWorker->read();
}

void LineSensor::stop()
{
	QMetaObject::invokeMethod(mLineSensorWorker.data(), "stop");
}
