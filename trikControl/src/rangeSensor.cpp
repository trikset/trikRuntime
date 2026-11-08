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
#include "port/iioDeviceResolver.h"
#include "port/portManager.h"

using namespace trikControl;

RangeSensor::RangeSensor(const QString &port, const trikKernel::Configurer &configurer, ModuleLoader &moduleLoader,
	const trikHal::HardwareAbstractionInterface &hardwareAbstraction, PortManager &ports)
	: mPortLease(ports.acquire(port, PortMode::Gpio, QStringLiteral("rangeSensor")))
	, mState("Range Sensor on " + port)
{
	if (!mPortLease) {
		mState.fail();
		return;
	}

	if (!moduleLoader.load(configurer.attributeByPort(port, "module"))
		|| !moduleLoader.load(configurer.attributeByDevice("rangeSensor", "commonModule"))) {
		QLOG_ERROR() << "Module loading failed";
		mState.fail();
		return;
	}

	mMinValue = ConfigurerHelper::configureInt(configurer, mState, port, "minValue");
	mMaxValue = ConfigurerHelper::configureInt(configurer, mState, port, "maxValue");

	const auto samplingFrequency = ConfigurerHelper::configureInt(configurer, mState, port, "samplingFrequency");
	if (samplingFrequency <= 0) {
		QLOG_ERROR() << "Invalid range sensor sampling frequency:" << samplingFrequency;
		mState.fail();
		return;
	}

	auto samplingPeriodMs = 1000 / samplingFrequency;
	if (samplingPeriodMs < 1) {
		// Fall back to the default 20 Hz sampling period.
		samplingPeriodMs = 50;
	}

	const auto iioLabel = configurer.attributeByPort(port, QStringLiteral("iioLabel"));
	const auto iioChannel = configurer.attributeByPort(port, QStringLiteral("iioChannel"));
	const auto sensorFile = findIioChannelFileByLabel(iioLabel, iioChannel);
	if (sensorFile.isEmpty()) {
		QLOG_ERROR() << "RangeSensor on" << port << "cannot resolve IIO channel" << iioLabel << iioChannel;
		mState.fail();
		return;
	}

	mSensorWorker.reset(new RangeSensorWorker(hardwareAbstraction, sensorFile, mState, mMinValue, mMaxValue,
		configurer.attributeByPort(port, "filter"), samplingPeriodMs));

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
		if (mState.isReady() || mState.isStarting()) {
			QMetaObject::invokeMethod(mSensorWorker.data(), &RangeSensorWorker::stop,
				Qt::BlockingQueuedConnection);
		}

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
	if (!mState.isFailed()) {
		QMetaObject::invokeMethod(mSensorWorker.data(), &RangeSensorWorker::init);
	}
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
