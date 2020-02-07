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

#include "gyroSensor.h"

#include <trikKernel/configurer.h>
#include <trikKernel/timeVal.h>
#include <QsLog.h>
#include <cmath>

#include "vectorSensorWorker.h"

using namespace trikControl;

static constexpr int GYRO_ARITHM_PRECISION = 0;
static constexpr auto GYRO_250DPS = 8.75f / (1 << GYRO_ARITHM_PRECISION) ;
static constexpr auto PI = 3.14159265358979323846f;
static constexpr auto PI_2 = 1.57079632679489661923f;
static constexpr auto RAD_TO_MDEG = 1000 * 180 / PI;

GyroSensor::GyroSensor(const QString &deviceName, const trikKernel::Configurer &configurer
		, const trikHal::HardwareAbstractionInterface &hardwareAbstraction, VectorSensorInterface *accelerometer)
	: mState(deviceName)
	, mIsCalibrated(false)
	, mQ(QQuaternion(1, 0, 0, 0))
	, mGyroCounter(0)
	, mLastUpdate(trikKernel::TimeVal(0, 0))
	, mAccelerometer(accelerometer)
	, mAxesSwapped(false)
{
	mVectorSensorWorker = new VectorSensorWorker(configurer.attributeByDevice(deviceName, "deviceFile"), mState
			, hardwareAbstraction, mWorkerThread);

	mBias.resize(3);
	mCalibrationValues.resize(6);
	mGyroSum.resize(3);

	mAccelerometerSum.resize(3);
	mAccelerometerCounter = 0;

	mCalibrationTimer.moveToThread(&mWorkerThread);
	mCalibrationTimer.setSingleShot(true);

	if (!mState.isFailed()) {
		qRegisterMetaType<trikKernel::TimeVal>("trikKernel::TimeVal");

		connect(mVectorSensorWorker, &VectorSensorWorker::newData, this, &GyroSensor::countTilt);

		connect(&mCalibrationTimer, &QTimer::timeout, this, &GyroSensor::countCalibrationParameters);

		QLOG_INFO() << "Starting VectorSensor worker thread" << &mWorkerThread;

		mState.ready();
	}
}

GyroSensor::~GyroSensor()
{
	mWorkerThread.quit();
	mWorkerThread.wait();
}

GyroSensor::Status GyroSensor::status() const
{
	return mState.status();
}

QVector<int> GyroSensor::read() const
{
	QReadLocker locker(&mResultLock);
	return mResult;
}

QVector<int> GyroSensor::readRawData() const
{
	return mRawData;
}

void GyroSensor::calibrate(int msec)
{
	connect(mVectorSensorWorker, &VectorSensorWorker::newData, this, &GyroSensor::sumGyroscope);
	connect(mAccelerometer, &VectorSensorInterface::newData, this, &GyroSensor::sumAccelerometer);

	mIsCalibrated = false;
	QMetaObject::invokeMethod(&mCalibrationTimer, [this, msec]() { mCalibrationTimer.start(msec); });
}

QVector<int> GyroSensor::getCalibrationValues()
{
	return mCalibrationValues;
}

void GyroSensor::setCalibrationValues(const QVector<int> &values)
{
	for (int i = 0; i < mBias.size(); i++) {
		mBias[i] = values[i];
	}

	QLOG_INFO() << "Gyro bias(raw): " << mBias;

	QVector3D acc(values[3], values[4], values[5]);
	acc.normalize();
	QLOG_INFO() << "Calibrated accelerometer: " << acc;

	QVector3D gravity(0, 0, 1);
	QVector3D delta = gravity - acc;

	mAxesSwapped = (delta.length() < 0.2f);
	float dot = QVector3D::dotProduct(acc, gravity);
	QVector3D cross = QVector3D::crossProduct(acc, gravity);

	mQ = QQuaternion::fromAxisAndAngle(cross, std::acos(dot) * 180 / PI);
	QLOG_INFO() << "Calibrated orientation sensor: Q = " << mQ;
}

bool GyroSensor::isCalibrated() const
{
	return mIsCalibrated;
}

void GyroSensor::countTilt(const QVector<int> &gyroData, trikKernel::TimeVal t)
{
	mRawData.resize(4);
	mRawData[0] = -gyroData[1];
	mRawData[1] = -gyroData[0];
	mRawData[2] = gyroData[2];
	mRawData[3] = t.packedUInt32();

	static bool timeInited = false;
	if (!timeInited) {
		timeInited = true;
		mLastUpdate = t;
	} else {

		const auto r0 = ((mRawData[0] << GYRO_ARITHM_PRECISION) - mBias[0]) * GYRO_250DPS;
		const auto r1 = ((mRawData[1] << GYRO_ARITHM_PRECISION) - mBias[1]) * GYRO_250DPS;
		const auto r2 = ((mRawData[2] << GYRO_ARITHM_PRECISION) - mBias[2]) * GYRO_250DPS;

		constexpr auto deltaConst = PI / 180 / 1000 / 1000000 / 2;
		const auto dt = (t - mLastUpdate) * deltaConst;

		auto x = r0 * dt;
		auto y = r1 * dt;
		auto z = r2 * dt;

		if (mAxesSwapped) {
			std::swap(x, z);
		}

		const auto c1 = std::cos(z);
		const auto s1 = std::sin(z);
		const auto c2 = std::cos(y);
		const auto s2 = std::sin(y);
		const auto c3 = std::cos(x);
		const auto s3 = std::sin(x);

		QQuaternion deltaQ;
		deltaQ.setScalar(c1 * c2 * c3 + s1 * s2 * s3);
		deltaQ.setX(s1 * c2 * c3 - c1 * s2 * s3);
		deltaQ.setY(c1 * s2 * c3 + s1 * c2 * s3);
		deltaQ.setZ(c1 * c2 * s3 - s1 * s2 * c3);

		mQ *= deltaQ;
		mQ.normalize();

		mLastUpdate = t;

		const QVector3D euler = getEulerAngles(mQ);

		QWriteLocker locker(&mResultLock);
		mResult.resize(7);
		mResult[0] = r0;
		mResult[1] = r1;
		mResult[2] = r2;
		mResult[3] = t.packedUInt32();

		mResult[4] = euler.x();
		mResult[5] = euler.z();
		mResult[6] = -euler.y();
		locker.unlock();

		emit newData(mResult, t);
	}
}

void GyroSensor::countCalibrationParameters()
{
	disconnect(mVectorSensorWorker, &VectorSensorWorker::newData, this, &GyroSensor::sumGyroscope);
	disconnect(mAccelerometer, &VectorSensorInterface::newData, this, &GyroSensor::sumAccelerometer);

	if (mGyroCounter != 0) {
		for (int i = 0; i < mGyroSum.size(); i++) {
			mCalibrationValues[i] = mGyroSum[i] / mGyroCounter;
			mGyroSum[i] = 0;
		}
		mGyroCounter = 0;
	}

	if (mAccelerometerCounter != 0) {
		for (int i = 0; i < mAccelerometerSum.size(); i++) {
			mCalibrationValues[i + 3] = mAccelerometerSum[i] / mAccelerometerCounter;
			mAccelerometerSum[i] = 0;
		}
		mAccelerometerCounter = 0;
	}

	setCalibrationValues(mCalibrationValues);

	mIsCalibrated = true;
	emit calibrationFinished();
}

void GyroSensor::sumAccelerometer(const QVector<int> &accelerometerData, const trikKernel::TimeVal &t)
{
	Q_UNUSED(t);
	mAccelerometerSum[0] += accelerometerData[0];
	mAccelerometerSum[1] += accelerometerData[1];
	mAccelerometerSum[2] += accelerometerData[2];

	mAccelerometerCounter++;
}

void GyroSensor::sumGyroscope(const QVector<int> &gyroData, const trikKernel::TimeVal &)
{
	mGyroSum[0] -= gyroData[1];
	mGyroSum[1] -= gyroData[0];
	mGyroSum[2] += gyroData[2];
	mGyroCounter++;
}

QVector3D GyroSensor::getEulerAngles(const QQuaternion &q)
{
	float pitch = 0.0;
	float roll = 0.0;
	float yaw = 0.0;

	const float x = q.x();
	const float y = q.y();
	const float z = q.z();
	const float w = q.scalar();

	float xx = x * x;
	float xy = x * y;
	float xz = x * z;
	float xw = x * w;
	float yy = y * y;
	float yz = y * z;
	float yw = y * w;
	float zz = z * z;
	float zw = z * w;

	const float squaredLength = xx + yy + zz + w * w;
	if (!qFuzzyIsNull(squaredLength - 1.0f) && !qFuzzyIsNull(squaredLength)) {
		   xx /= squaredLength;
		   xy /= squaredLength;
		   xz /= squaredLength;
		   xw /= squaredLength;
		   yy /= squaredLength;
		   yz /= squaredLength;
		   yw /= squaredLength;
		   zz /= squaredLength;
		   zw /= squaredLength;
	   }

	   pitch = std::asin(-2.0f * (yz - xw));
	   if (pitch < PI_2) {
		   if (pitch > -PI_2) {
			   yaw = std::atan2(2.0f * (xz + yw), 1.0f - 2.0f * (xx + yy));
			   roll = std::atan2(2.0f * (xy + zw), 1.0f - 2.0f * (xx + zz));
		   } else {
			   // not a unique solution
			   roll = 0.0f;
			   yaw = -std::atan2(-2.0f * (xy - zw), 1.0f - 2.0f * (yy + zz));
		   }
	   } else {
		   // not a unique solution
		   roll = 0.0f;
		   yaw = std::atan2(-2.0f * (xy - zw), 1.0f - 2.0f * (yy + zz));
	   }

	   pitch = pitch * RAD_TO_MDEG;
	   yaw = yaw * RAD_TO_MDEG;
	   roll = roll * RAD_TO_MDEG;
	   return QVector3D(pitch, roll, yaw);
}
