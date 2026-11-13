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

#pragma once

#include <array>

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>

#include <trikHal/IIOFileInterface.h>

#include "gyroSensor.h"
#include "vectorSensor.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

/// Owns a single IIO stream that contains both accelerometer and gyroscope
/// samples and exposes it through the regular sensor interfaces.
///
/// The new board has one combined IMU device, while the public Brick API still
/// provides separate accelerometer() and gyroscope() objects. BoardImu hides
/// that difference: it reads one generic IIO stream, checks that the required
/// channels are present, splits every sample into accelerometer and gyroscope
/// axes, and forwards both parts with the shared timestamp.
class BoardImu : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - configuration port that provides the `deviceFile`
	/// attribute of the IIO character device.
	/// @param configurer - configurer with the board configuration.
	/// @param hardwareAbstraction - hardware abstraction used to create the
	/// IIO file implementation.
	BoardImu(const QString &port, const trikKernel::Configurer &configurer,
		const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	~BoardImu() override;

	/// Returns the accelerometer backed by the shared IIO stream.
	/// The returned object is owned by BoardImu and is null if the stream could
	/// not be opened or validated.
	VectorSensorInterface *accelerometer() const;

	/// Returns the gyroscope backed by the shared IIO stream.
	/// The returned object is owned by BoardImu and is null if the stream could
	/// not be opened or validated.
	GyroSensorInterface *gyroscope() const;

	/// Returns the raw temperature value from the last received sample.
	int temperature() const;

private Q_SLOTS:
	/// Splits a combined IMU sample and feeds the accelerometer and gyroscope.
	void onImuData(const trikHal::IIOSample &sample);

private:
	/// Checks that the opened device provides the accelerometer, gyroscope,
	/// temperature and timestamp channels, and remembers their value indices.
	bool validateLayout(QString *error);

	QScopedPointer<trikHal::IIOFileInterface> mImuFile {};
	QScopedPointer<VectorSensor> mAccelerometer {};
	QScopedPointer<GyroSensor> mGyroscope {};
	int mTemperature = 0;

	std::array<int, 3> mAccelValueIndex {{-1, -1, -1}};
	std::array<int, 3> mGyroValueIndex {{-1, -1, -1}};
	int mTempValueIndex = -1;
};

}
