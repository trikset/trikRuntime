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

#include <QtCore/QObject>
#include <QtCore/QVector>

#include <trikKernel/timeVal.h>

#include <trikHal/trikHalDeclSpec.h>

namespace trikHal {

/// A single measurement read from a combined accelerometer/gyroscope IIO
/// device.
///
/// The device used by the new board exposes both sensors in one IIO device.
/// Consequently, one sample contains both sensors, one shared temperature
/// value and one shared timestamp. It is not possible to associate different
/// timestamps with the accelerometer and the gyroscope.
struct ImuSample {
	/// Accelerometer axes in raw IIO units, in X, Y, Z order.
	QVector<int> accel;

	/// Gyroscope axes in raw IIO units, in X, Y, Z order.
	QVector<int> gyro;

	/// Temperature in raw IIO units. The value is shared by both sensors.
	int temperature = 0;

	/// Timestamp supplied by the IIO subsystem for this sample.
	trikKernel::TimeVal time {0, 0};
};

/// Interface of a combined IMU IIO character device.
///
/// Implementations are expected to open a device such as
/// `/dev/iio:device0`, validate that the required scan elements are enabled
/// and emit newImuData() for every complete sample read from the device.
///
/// Implementations must not block the event loop: open() prepares the device
/// for event-driven reading, close() releases it, and newImuData() is emitted
/// from the thread that owns the underlying notifier.
class TRIKHAL_EXPORT ImuFileInterface : public QObject
{
	Q_OBJECT

public:
	~ImuFileInterface() override = default;

	/// Opens the IIO device and starts listening for samples.
	/// @returns true on success, false if the device cannot be opened or its
	/// scan layout does not match the expected combined IMU layout.
	virtual bool open() = 0;

	/// Stops listening for samples and releases the device.
	/// @returns true if the device was opened and closed successfully.
	virtual bool close() = 0;

	/// Returns the path of the opened IIO character device.
	virtual QString fileName() const = 0;

	/// Returns true if the device is currently open.
	virtual bool isOpened() const = 0;

Q_SIGNALS:
	/// Emitted for every complete IMU sample.
	/// @param sample - accelerometer, gyroscope, temperature and timestamp.
	void newImuData(const trikHal::ImuSample &sample);
};

}
