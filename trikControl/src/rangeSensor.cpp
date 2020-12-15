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
#include <trikKernel/timeVal.h>
#include <QEventLoop>
#include <QsLog.h>

#include "configurerHelper.h"
#include "rangeSensorWorker.h"
#include "moduleLoader.h"

using namespace trikControl;

RangeSensor::RangeSensor(const QString &port, const trikKernel::Configurer &configurer, ModuleLoader &moduleLoader
		, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mState("Range Sensor on " + port)
{
	if (!moduleLoader.load(configurer.attributeByPort(port, "module"))
			|| !moduleLoader.load(configurer.attributeByDevice("rangeSensor", "commonModule")))
	{
		QLOG_ERROR() << "Module loading failed";
		mState.fail();
		return;
	}

	mMinValue = ConfigurerHelper::configureInt(configurer, mState, port, "minValue");
	mMaxValue = ConfigurerHelper::configureInt(configurer, mState, port, "maxValue");

	mSensorWorker.reset(new RangeSensorWorker(configurer.attributeByPort(port, "eventFile"), mState
			, hardwareAbstraction, mMinValue, mMaxValue, configurer.attributeByPort(port, "filter")));

	if (!mState.isFailed()) {
		mSensorWorker->moveToThread(&mWorkerThread);

		connect(mSensorWorker.data(), &RangeSensorWorker::newData, this, &RangeSensor::newData);

		QLOG_INFO() << "Starting RangeSensor worker thread" << &mWorkerThread;
		QEventLoop l;
		connect(&mWorkerThread, &QThread::started, &l, &QEventLoop::quit);

		mWorkerThread.setObjectName(mSensorWorker->metaObject()->className());
		mWorkerThread.start();

		l.exec();
	}
}

RangeSensor::~RangeSensor()
{
	if (mWorkerThread.isRunning()) {
		mWorkerThread.quit();
		mWorkerThread.wait();
	}
}

RangeSensor::Status RangeSensor::status() const
{
	return mState.status();
}

void RangeSensor::init()
{
	if (mState.isFailed()) {
		mState.resetFailure();
	}

	QMetaObject::invokeMethod(mSensorWorker.data(), &RangeSensorWorker::init);
}

int RangeSensor::read()
{
	if (!mState.isFailed()) {
		// Read is called synchronously and only takes prepared value from sensor.
		return mSensorWorker->read();
	} else {
		return -1;
	}
}

int RangeSensor::readRawData()
{
	if (!mState.isFailed()) {
		// Read is called synchronously and only takes prepared value from sensor.
		return mSensorWorker->readRawData();
	} else {
		return -1;
	}
}

void RangeSensor::stop()
{
	if (!mState.isFailed()) {
		QMetaObject::invokeMethod(mSensorWorker.data(), &RangeSensorWorker::stop);
	}
}

int RangeSensor::minValue() const
{
	return mMinValue;
}

int RangeSensor::maxValue() const
{
	return mMaxValue;
}
