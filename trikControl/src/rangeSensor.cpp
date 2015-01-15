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

#include "rangeSensor.h"

#include <QtCore/QThread>

#include <trikKernel/configurer.h>

#include "rangeSensorWorker.h"

using namespace trikControl;

RangeSensor::RangeSensor(QString const &port, trikKernel::Configurer const &configurer)
	: mSensorWorker(new RangeSensorWorker(configurer.attributeByPort(port, "eventFile")))
{
	mSensorWorker->moveToThread(&mWorkerThread);

	connect(mSensorWorker.data(), SIGNAL(newData(int, int)), this, SIGNAL(newData(int, int)));

	mWorkerThread.start();
}

RangeSensor::~RangeSensor()
{
	mWorkerThread.quit();
	mWorkerThread.wait();
}

void RangeSensor::init()
{
	QMetaObject::invokeMethod(mSensorWorker.data(), "init");
}

int RangeSensor::read()
{
	// Read is called synchronously and only takes prepared value from sensor.
	return mSensorWorker->read();
}

int RangeSensor::readRawData()
{
	// Read is called synchronously and only takes prepared value from sensor.
	return mSensorWorker->readRawData();
}

void RangeSensor::stop()
{
	QMetaObject::invokeMethod(mSensorWorker.data(), "stop");
}
