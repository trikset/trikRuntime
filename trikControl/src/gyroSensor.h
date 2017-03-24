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
#include <cmath>
#include <trikKernel/timeVal.h>

#include "gyroSensorInterface.h"
#include "deviceState.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

class VectorSensorWorker;

/// Sensor that returns a vector.
class GyroSensor : public GyroSensorInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - port on which this sensor is configured.
	/// @param configurer - configurer object containing preparsed XML files with sensor parameters.
	GyroSensor(const QString &deviceName, const trikKernel::Configurer &configurer
			, const trikHal::HardwareAbstractionInterface &hardwareAbstraction, VectorSensorInterface *accelerometer);

	~GyroSensor() override;

	Status status() const override;

public slots:
	QVector<int> read() const override;

	QVector<int> readRawData() const override;

	void calibrate(int msec) override;

	bool isCalibrated() const override;

private slots:
	/// Counts current angle velocities (3-axis) in mdps, current tilts (3-axis) in mdps
	/// and packed time of current event.
	void countTilt(const QVector<int> &gyroData, trikKernel::TimeVal t);

	/// Calculates average mean of bias and reset other tilt parameters.
	void initParameters();

	void sumAccelerometer(const QVector<int> &accelerometerData, const trikKernel::TimeVal &);

	/// Sums values of bias.
	void sumGyroscope(const QVector<int> &gyroData, const trikKernel::TimeVal &);

	QVector3D getEulerAngles(const QQuaternion &q);

private:
	/// Device state, shared with worker.
	DeviceState mState;

	QScopedPointer<VectorSensorWorker> mVectorSensorWorker;
	QThread mWorkerThread;

	QTimer mCalibrationTimer;
	bool mIsCalibrated;

	/// Quaternion that presented current rotation.
	QQuaternion mQ;

	/// Vector of average means of bias (3-axis).
	QVector<int> mBias;

	/// Vector for collecting bias sums.
	QVector<int> mGyroSum;

	/// Counter for bias sums.
	int mGyroCounter;

	/// Result vector consists of:
	/// [0-2] parameters - angular velocities (3-axis);
	/// [3] parameter - packed data of evet time;
	/// [4-6] parameters - tilts (3-axis).
	QVector<int> mResult;

	/// Raw values of gyroscope data.
	QVector<int> mRawData;

	/// Timestamp of last gyroscope data.
	trikKernel::TimeVal mLastUpdate;

	VectorSensorInterface *mAccelerometer;

	QVector<int> mAccelerometerVector;
	QVector<int> mAccelerometerSum;
	int mAccelerometerCounter;

	bool mAxesSwapped;
};

}
