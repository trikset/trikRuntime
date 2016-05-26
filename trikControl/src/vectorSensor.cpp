/* Copyright 2013 - 2015 Matvey Bryksin, Yurii Litvinov and CyberTech Labs Ltd.
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

#include "vectorSensor.h"

#include <trikKernel/configurer.h>
#include <trikKernel/timeVal.h>
#include <QsLog.h>

#include "vectorSensorWorker.h"

using namespace trikControl;

VectorSensor::VectorSensor(const QString &deviceName, const trikKernel::Configurer &configurer
		, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mState(deviceName)
{
	mVectorSensorWorker.reset(new VectorSensorWorker(configurer.attributeByDevice(deviceName, "deviceFile"), mState
			, hardwareAbstraction, mWorkerThread));

	if (!mState.isFailed()) {
		qRegisterMetaType<trikKernel::TimeVal>("trikKernel::TimeVal");
		connect(mVectorSensorWorker.data(), SIGNAL(newData(QVector<int>,trikKernel::TimeVal))
				, this, SIGNAL(newData(QVector<int>,trikKernel::TimeVal)));

		QLOG_INFO() << "Starting VectorSensor worker thread" << &mWorkerThread;

		mState.ready();
	}
}

VectorSensor::~VectorSensor()
{
	if (mWorkerThread.isRunning()) {
		QMetaObject::invokeMethod(mVectorSensorWorker.data(), "deinitialize");
		mWorkerThread.quit();
		mWorkerThread.wait();
	}
}

VectorSensor::Status VectorSensor::status() const
{
	return mState.status();
}

QVector<int> VectorSensor::read() const
{
	return mVectorSensorWorker->read();
}
