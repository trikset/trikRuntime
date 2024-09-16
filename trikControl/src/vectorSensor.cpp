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
	, const trikHal::HardwareAbstractionInterface &hardwareAbstraction
	, const QString &port)
	: mState(deviceName)
{
	qRegisterMetaType<trikKernel::TimeVal>("trikKernel::TimeVal");

#ifndef TRIK_IIO_ACCEL_GYRO
	Q_UNUSED(port)
	mVectorSensorWorker = new VectorSensorWorker(configurer.attributeByDevice(deviceName, "deviceFile"), mState
		, hardwareAbstraction);
	mVectorSensorWorker->moveToThread(&mWorkerThread);

	connect(&mWorkerThread, &QThread::started, mVectorSensorWorker, &VectorSensorWorker::init);
	connect(&mWorkerThread, &QThread::finished, mVectorSensorWorker, &VectorSensorWorker::deleteLater);

	mWorkerThread.setObjectName(mVectorSensorWorker->metaObject()->className());
	mWorkerThread.start();

	if (!mState.isFailed()) {
		connect(mVectorSensorWorker, &VectorSensorWorker::newData, this, &VectorSensor::newData);
#else /* ! TRIK_IIO_ACCEL_GYRO */
	if (!mState.isFailed()) {
		mIIOFile.reset(hardwareAbstraction.createIIOFile(configurer.attributeByPort(port, "deviceFile"),
			configurer.attributeByPort(port, "scanType")));
		if (!mIIOFile.data()->open()) {
			QLOG_ERROR() << "Gyroscope init failed";
			mState.fail();
			return;
		}
		connect(mIIOFile.data(), &trikHal::IIOFileInterface::newData, this, &VectorSensor::newData);
		connect(mIIOFile.data(), &trikHal::IIOFileInterface::newData
			, this, [this](QVector<int> reading, const trikKernel::TimeVal &eventTime) {
			Q_UNUSED(eventTime);
			mResult = reading;
		});
#endif /* TRIK_IIO_ACCEL_GYRO */

		QLOG_INFO() << "Starting VectorSensor";
		mState.ready();
	}
}

VectorSensor::~VectorSensor()
{
	mWorkerThread.quit();
	mWorkerThread.wait();
}

VectorSensor::Status VectorSensor::status() const
{
	return mState.status();
}

QVector<int> VectorSensor::read() const
{
#ifndef TRIK_IIO_ACCEL_GYRO
	return mVectorSensorWorker->read();
#else /* TRIK_IIO_ACCEL_GYRO */
	return mResult;
#endif /* TRIK_IIO_ACCEL_GYRO */
}
