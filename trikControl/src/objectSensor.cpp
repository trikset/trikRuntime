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

#include "objectSensor.h"

#include "src/objectSensorWorker.h"

#include <QtCore/QDebug>

using namespace trikControl;

ObjectSensor::ObjectSensor(QString const &script, QString const &inputFile, QString const &outputFile
		, double toleranceFactor)
	: mObjectSensorWorker(new ObjectSensorWorker(script, inputFile, outputFile, toleranceFactor))
{
	mObjectSensorWorker->moveToThread(&mWorkerThread);

	connect(mObjectSensorWorker.data(), SIGNAL(stopped()), this, SIGNAL(stopped()));

	mWorkerThread.start();
}

ObjectSensor::~ObjectSensor()
{
	mWorkerThread.quit();
	mWorkerThread.wait();
}

void ObjectSensor::init(bool showOnDisplay)
{
	QMetaObject::invokeMethod(mObjectSensorWorker.data(), "init", Q_ARG(bool, showOnDisplay));
}

void ObjectSensor::detect()
{
	QMetaObject::invokeMethod(mObjectSensorWorker.data(), "detect");
}

QVector<int>  ObjectSensor::read()
{
	// Read is called synchronously and only takes prepared value from sensor.
	return mObjectSensorWorker->read();
}

void ObjectSensor::stop()
{
	QMetaObject::invokeMethod(mObjectSensorWorker.data(), "stop");
}
