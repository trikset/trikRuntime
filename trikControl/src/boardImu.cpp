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

#include <array>

#include <trikKernel/configurer.h>
#include <trikHal/hardwareAbstractionInterface.h>

#include <QsLog.h>

using namespace trikControl;

BoardImu::BoardImu(const QString &port, const trikKernel::Configurer &configurer,
	const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: QObject(nullptr)
{
	const auto deviceFile = configurer.attributeByPort(port, "deviceFile");

	mImuFile.reset(hardwareAbstraction.createIIOFile(deviceFile));
	if (!mImuFile->open()) {
		QLOG_ERROR() << "BoardImu: failed to open" << deviceFile;
		return;
	}

	auto error = QString();
	if (!validateLayout(&error)) {
		QLOG_ERROR() << "BoardImu: layout validation failed for" << deviceFile << ":" << error;
		mImuFile->close();
		mImuFile.reset();
		return;
	}

	mAccelerometer.reset(new VectorSensor("accelerometer"));
	mGyroscope.reset(new GyroSensor("gyroscope", deviceFile, mAccelerometer.data()));

	connect(mImuFile.data(), &trikHal::IIOFileInterface::newData, this, &BoardImu::onImuData);
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

bool BoardImu::validateLayout(QString *error)
{
	const auto channels = mImuFile->channels();

	const auto find = [&channels](trikHal::IIOChanType type,
				  trikHal::IIOAxis axis) -> const trikHal::IIOChannelInfo * {
		for (const auto &channel : channels) {
			if (channel.id.type == type && channel.id.axis == axis) {
				return &channel;
			}
		}

		return nullptr;
	};

	const std::array<trikHal::IIOAxis, 3> axes {trikHal::IIOAxis::X, trikHal::IIOAxis::Y, trikHal::IIOAxis::Z};

	const auto validateAxes = [&](trikHal::IIOChanType type, const QString &name,
					  std::array<int, 3> *indices) -> bool {
		auto reference = trikHal::IIOChannelFormat {};
		for (auto i = 0; i < 3; ++i) {
			const auto *channel = find(type, axes[i]);
			if (!channel) {
				*error = QStringLiteral("missing %1 channel").arg(name);
				return false;
			}

			if (channel->format.storageBits != 16) {
				*error = QStringLiteral("%1 channels must be 16-bit").arg(name);
				return false;
			}

			if (i == 0) {
				reference = channel->format;
			} else if (channel->format != reference) {
				*error = QStringLiteral("%1 channels have different scan types").arg(name);
				return false;
			}

			(*indices)[i] = mImuFile->channelValueIndex(type, axes[i]);
			if ((*indices)[i] < 0) {
				*error = QStringLiteral("%1 channel is not exposed by the reader").arg(name);
				return false;
			}
		}

		return true;
	};

	if (!validateAxes(trikHal::IIOChanType::Accel, QStringLiteral("accelerometer"), &mAccelValueIndex)) {
		return false;
	}

	if (!validateAxes(trikHal::IIOChanType::AnglVel, QStringLiteral("gyroscope"), &mGyroValueIndex)) {
		return false;
	}

	const auto *temperature = find(trikHal::IIOChanType::Temp, trikHal::IIOAxis::None);
	if (!temperature) {
		*error = QStringLiteral("missing temperature channel");
		return false;
	}

	if (temperature->format.storageBits != 8 && temperature->format.storageBits != 16) {
		*error = QStringLiteral("unexpected temperature channel format");
		return false;
	}

	mTempValueIndex = mImuFile->channelValueIndex(trikHal::IIOChanType::Temp, trikHal::IIOAxis::None);
	if (mTempValueIndex < 0) {
		*error = QStringLiteral("temperature channel is not exposed by the reader");
		return false;
	}

	if (!mImuFile->hasTimestamp()) {
		*error = QStringLiteral("missing timestamp channel");
		return false;
	}

	return true;
}

void BoardImu::onImuData(const trikHal::IIOSample &sample)
{
	const auto size = sample.values.size();

	for (auto i = 0; i < 3; ++i) {
		if (mAccelValueIndex[i] < 0 || mAccelValueIndex[i] >= size || mGyroValueIndex[i] < 0
			|| mGyroValueIndex[i] >= size) {
			QLOG_WARN() << "BoardImu: unexpected sample size:" << size;
			return;
		}
	}

	if (mTempValueIndex < 0 || mTempValueIndex >= size) {
		QLOG_WARN() << "BoardImu: unexpected sample size:" << size;
		return;
	}

	mTemperature = sample.values.at(mTempValueIndex);

	QVector<int> accel {
		sample.values.at(mAccelValueIndex[0]),
		sample.values.at(mAccelValueIndex[1]),
		sample.values.at(mAccelValueIndex[2]),
	};

	QVector<int> gyro {
		sample.values.at(mGyroValueIndex[0]),
		sample.values.at(mGyroValueIndex[1]),
		sample.values.at(mGyroValueIndex[2]),
	};

	if (mAccelerometer) {
		mAccelerometer->onNewData(accel, sample.time);
	}

	if (mGyroscope) {
		mGyroscope->onNewData(gyro, sample.time);
	}
}
