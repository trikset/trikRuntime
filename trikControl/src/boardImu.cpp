/* Copyright 2026 CyberTech Labs Ltd.
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

#include "boardImu.h"

#include <trikKernel/configurer.h>
#include <trikHal/hardwareAbstractionInterface.h>

#include <QsLog.h>

using namespace trikControl;

BoardImu::BoardImu(const QString &port, const trikKernel::Configurer &configurer,
	const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: QObject(nullptr)
{
	const auto deviceFile = configurer.attributeByPort(port, "deviceFile");

	mImuFile.reset(hardwareAbstraction.createImuFile(deviceFile));
	if (!mImuFile->open()) {
		QLOG_ERROR() << "BoardImu: failed to open" << deviceFile;
		return;
	}

	mAccelerometer.reset(new VectorSensor("accelerometer"));
	mGyroscope.reset(new GyroSensor("gyroscope", deviceFile, mAccelerometer.data()));

	connect(mImuFile.data(), &trikHal::ImuFileInterface::newImuData, this, &BoardImu::onImuData);
}

BoardImu::~BoardImu()
{
	if (mImuFile && mImuFile->isOpened()) {
		mImuFile->close();
	}
}

VectorSensorInterface *BoardImu::accelerometer() const
{
	return mAccelerometer.data();
}

GyroSensorInterface *BoardImu::gyroscope() const
{
	return mGyroscope.data();
}

int BoardImu::temperature() const
{
	return mTemperature;
}

void BoardImu::onImuData(const trikHal::ImuSample &sample)
{
	if (sample.accel.size() != 3 || sample.gyro.size() != 3) {
		QLOG_WARN() << "BoardImu: unexpected sample size, accel:" << sample.accel.size()
			    << "gyro:" << sample.gyro.size();
		return;
	}

	mTemperature = sample.temperature;

	if (mAccelerometer) {
		mAccelerometer->onNewData(sample.accel, sample.time);
	}

	if (mGyroscope) {
		mGyroscope->onNewData(sample.gyro, sample.time);
	}
}
