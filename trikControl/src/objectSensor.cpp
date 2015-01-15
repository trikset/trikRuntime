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

#include "objectSensorWorker.h"

using namespace trikControl;

ObjectSensor::ObjectSensor(QString const &port, trikKernel::Configurer const &configurer)
{
	QString const &script = configurer.attributeByPort(port, "script");
	QString const &inputFile = configurer.attributeByPort(port, "inputFile");
	QString const &outputFile = configurer.attributeByPort(port, "outputFile");
	double const toleranceFactor = configurer.attributeByPort(port, "toleranceFactor").toDouble();

	mObjectSensorWorker.reset(new ObjectSensorWorker(script, inputFile, outputFile, toleranceFactor));

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
