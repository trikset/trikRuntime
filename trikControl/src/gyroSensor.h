/* Copyright 2016 Anastasiya Kornilova.
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

#include <QtCore/QScopedPointer>
#include <QtCore/QThread>
#include <QQuaternion>
#include <QTimer>
#include <QReadWriteLock>
#include <cmath>
#include <trikKernel/timeVal.h>
#include <trikHal/hardwareAbstractionInterface.h>

#include "gyroSensorInterface.h"
#include "deviceState.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
class IIOFileInterface;
}

namespace trikControl {

/// Sensor that returns a vector.
class GyroSensor : public GyroSensorInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - port on which this sensor is configured.
	/// @param configurer - configurer object containing preparsed XML files with sensor parameters.
	GyroSensor(const QString &deviceName, const trikKernel::Configurer &configurer,
		const trikHal::HardwareAbstractionInterface &hardwareAbstraction, VectorSensorInterface *accelerometer,
		const QString &port);

	/// Constructor for externally-fed sensor (used by BoardImu). Does not open its own IIO device.
	/// @param deviceName - sensor name.
	/// @param deviceFile - path to the IIO char device (needed to read gyro scale from sysfs).
	/// @param accelerometer - pointer to the accelerometer sensor, used for calibration.
	GyroSensor(const QString &deviceName, const QString &deviceFile, VectorSensorInterface *accelerometer);

	~GyroSensor() override;

	Status status() const override;

	Q_INVOKABLE QVector<int> read() const override;

	Q_INVOKABLE QVector<int> readRawData() const override;

	Q_INVOKABLE QVector<int> getCalibrationValues() override;

	Q_INVOKABLE bool isCalibrated() const override;

	Q_SLOT void calibrate(int msec) override;

	Q_SLOT void setCalibrationValues(const QVector<int> &values) override;

public Q_SLOTS:
	/// Feeds data from a shared IIO source (BoardImu). Replaces internal IIO file signal.
	void onNewData(const QVector<int> &data, const trikKernel::TimeVal &eventTime);

private Q_SLOTS:
	/// Counts current angle velocities (3-axis) in mdps, current tilts (3-axis) in mdps
	/// and packed time of current event.
	void countTilt(const QVector<int> &gyroData, trikKernel::TimeVal t);

	/// Calculates average mean of bias and reset other tilt parameters.
	void countCalibrationParameters();

	void sumAccelerometer(const QVector<int> &accelerometerData, const trikKernel::TimeVal &t);

	/// Sums values of bias.
	void sumGyroscope(const QVector<int> &gyroData, const trikKernel::TimeVal &);

private:
	QVector3D getEulerAngles(const QQuaternion &q);

	/// Reads gyro scale (in_anglvel_scale) from sysfs and converts it to mdps per LSB.
	bool readGyroScale(const QString &deviceFile, bool required);

	/// Initialises members common to both constructors (bias arrays, timers, meta types).
	void initCommon();

	/// Device state, shared with worker.
	DeviceState mState;

	QThread mWorkerThread;

	QScopedPointer<trikHal::IIOFileInterface> mIIOFile;

	QTimer mCalibrationTimer;
	bool mIsCalibrated;

	/// Quaternion that presented current rotation.
	QQuaternion mQ;

	/// Gyro scale in mdps per LSB, read from IIO in_anglvel_scale.
	double mGyroScale {0.0};

	/// Vector of average means of bias (3-axis).
	QVector<int> mBias;

	QVector<int> mCalibrationValues;

	/// Vector for collecting bias sums.
	QVector<int> mGyroSum;

	/// Counter for bias sums.
	int mGyroCounter;

	/// Result vector consists of:
	/// [0-2] parameters - angular velocities (3-axis);
	/// [3] parameter - packed data of event time;
	/// [4-6] parameters - tilts (3-axis).
	QVector<int> mResult;

	/// Raw values of gyroscope data.
	QVector<int> mRawData;

	/// Timestamp of last gyroscope data.
	trikKernel::TimeVal mLastUpdate;

	VectorSensorInterface *mAccelerometer; // Has no ownership

	QVector<int> mAccelerometerVector;
	QVector<int> mAccelerometerSum;
	int mAccelerometerCounter = 0;

	bool mAxesSwapped;

	mutable QReadWriteLock mResultLock;
};

}