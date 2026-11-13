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

#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QReadWriteLock>
#include <QtCore/QScopedPointer>
#include <QtCore/QTimer>

#include <trikHal/IIOFileInterface.h>

#include "magnetometerSensorInterface.h"
#include "deviceState.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

/// 3-axis magnetometer backed by a generic IIO character device.
///
/// Opens the device, checks that the magnetometer X/Y/Z channels are present,
/// reads decoded samples and stores both raw and (optionally) hard/soft-iron
/// corrected values.
class MagnetometerSensor : public MagnetometerSensorInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - configuration port that provides the `deviceFile`
	/// attribute of the IIO character device.
	/// @param configurer - configurer with the board configuration.
	/// @param hardwareAbstraction - hardware abstraction used to create the
	/// IIO file implementation.
	MagnetometerSensor(const QString &port, const trikKernel::Configurer &configurer,
		const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	~MagnetometerSensor() override;

	Status status() const override;

	QVector<int> read() const override;

	QVector<int> readRawData() const override;

	void calibrate(int msec) override;

	void setCalibrationValues(const QVector<double> &values) override;

	QVector<double> getCalibrationValues() override;

	bool isCalibrated() const override;

private Q_SLOTS:
	/// Stores one decoded sample, applying calibration when available.
	void onNewData(const trikHal::IIOSample &sample);

private:
	/// Checks that the opened device provides the magnetometer X/Y/Z channels
	/// and remembers their value indices.
	bool validateLayout(QString *error);

	/// Applies hard/soft iron correction to a raw sample. Caller must hold the lock.
	QVector<int> applyCalibration(const QVector<int> &raw) const;

	/// Stores a calibration sample unless it is too close to the last one.
	/// Caller must hold mCalibrationLock. Returns true if the sample was stored.
	bool storeCalibrationSample(const QVector<int> &raw);

	/// Internal implementation of the calibration run.
	void calibrateInternal();

	DeviceState mState;
	QScopedPointer<trikHal::IIOFileInterface> mIioFile;

	std::array<int, 3> mValueIndex {{-1, -1, -1}};

	QVector<int> mRaw;
	QVector<int> mResult;

	/// Calibration values layout: [0..2] hard iron, [3..11] soft iron.
	QVector<double> mCalibrationValues;

	bool mIsCalibrated = false;

	/// Number of raw samples collected during one calibration run.
	static constexpr int kCalibrationSamples = 1000;

	/// Minimum distance (in raw LSB, per axis) between consecutive stored samples.
	/// Used to drop near-duplicate samples so the buffer keeps a diverse cloud.
	static constexpr int kCalibrationMinDelta = 10;

	/// One raw magnetometer reading collected for calibration.
	struct CalibrationSample {
		qint32 x = 0;
		qint32 y = 0;
		qint32 z = 0;
	};

	std::array<CalibrationSample, kCalibrationSamples> mCalibrationSamples {};
	int mCollectedSamples = 0;
	bool mCalibrating = false;
	QTimer mCalibrationTimer;
	mutable QMutex mCalibrationLock;

	mutable QReadWriteLock mResultLock;
};

}
