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

using namespace trikControl;

static constexpr auto PI = 3.14159265358979323846f;
static constexpr auto PI_2 = 1.57079632679489661923f;
static constexpr auto RAD_TO_MDEG = 1000 * 180 / PI;

GyroSensor::GyroSensor(const QString &deviceName, const trikKernel::Configurer &configurer,
	const trikHal::HardwareAbstractionInterface &hardwareAbstraction, VectorSensorInterface *accelerometer,
	const QString &port)
	: mState(deviceName)
	, mIsCalibrated(false)
	, mQ(QQuaternion(1, 0, 0, 0))
	, mGyroCounter(0)
	, mLastUpdate(trikKernel::TimeVal(0, 0))
	, mAccelerometer(accelerometer)
	, mAxesSwapped(false)
{
	initCommon();

	if (!mState.isFailed()) {
		const auto deviceFile = configurer.attributeByPort(port, "deviceFile");
		mIIOFile.reset(
			hardwareAbstraction.createIIOFile(deviceFile, configurer.attributeByPort(port, "scanType")));
		if (!mIIOFile.data()->open()) {
			QLOG_ERROR() << "Gyroscope init failed";
			mState.fail();
			return;
		}

		if (!readGyroScale(deviceFile, false)) {
			QLOG_ERROR() << "Gyroscope scale init failed";
			mState.fail();
			return;
		}

		connect(mIIOFile.data(), &trikHal::IIOFileInterface::newData, this, &GyroSensor::countTilt);
		connect(&mCalibrationTimer, &QTimer::timeout, this, &GyroSensor::countCalibrationParameters);
		QLOG_INFO() << "Starting GyroSensor";
		mState.ready();
	}
}

GyroSensor::GyroSensor(const QString &deviceName, const QString &deviceFile, VectorSensorInterface *accelerometer)
	: mState(deviceName)
	, mIsCalibrated(false)
	, mQ(QQuaternion(1, 0, 0, 0))
	, mGyroCounter(0)
	, mLastUpdate(trikKernel::TimeVal(0, 0))
	, mAccelerometer(accelerometer)
	, mAxesSwapped(false)
{
	initCommon();

	if (!readGyroScale(deviceFile, true)) {
		QLOG_ERROR() << "Gyroscope scale init failed";
		mState.fail();
		return;
	}

	connect(&mCalibrationTimer, &QTimer::timeout, this, &GyroSensor::countCalibrationParameters);
	QLOG_INFO() << "Starting GyroSensor (shared IIO)";
	mState.ready();
}

void GyroSensor::initCommon()
{
	mBias.resize(3);
	mCalibrationValues.resize(6);
	mGyroSum.resize(3);

	mAccelerometerSum.resize(3);
	mAccelerometerCounter = 0;

	mCalibrationTimer.setSingleShot(true);
	qRegisterMetaType<trikKernel::TimeVal>("trikKernel::TimeVal");
}

GyroSensor::~GyroSensor()
{
	mWorkerThread.quit();
	mWorkerThread.wait();
}

bool GyroSensor::readGyroScale(const QString &deviceFile, bool required)
{
	auto sysfsDevice = deviceFile;
	sysfsDevice.replace(QLatin1String("/dev/"), QLatin1String("/sys/bus/iio/devices/"));
	const QString scaleFile = sysfsDevice + QLatin1String("/in_anglvel_scale");

	QFile file(scaleFile);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		if (required) {
			QLOG_ERROR() << "Can't open" << scaleFile;
			return false;
		}

		QLOG_WARN() << "Can't open" << scaleFile << ", using default gyro scale";
		mGyroScale = 8.75;
		return true;
	}

	bool ok = false;
	const double scaleRadPerSec = QString::fromUtf8(file.readAll()).trimmed().toDouble(&ok);
	if (!ok) {
		if (required) {
			QLOG_ERROR() << "Can't parse gyro scale from" << scaleFile;
			return false;
		}

		QLOG_WARN() << "Can't parse gyro scale from" << scaleFile << ", using default gyro scale";
		mGyroScale = 8.75;
		return true;
	}

	mGyroScale = scaleRadPerSec * RAD_TO_MDEG;
	QLOG_INFO() << "Gyro scale:" << mGyroScale << "mdps per LSB";
	return true;
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
	connect(mIIOFile.data(), &trikHal::IIOFileInterface::newData, this, &GyroSensor::sumGyroscope);
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

	QVector3D acc(static_cast<float>(values[3]), static_cast<float>(values[4]), static_cast<float>(values[5]));
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
	mRawData = gyroData;
	mRawData.resize(4);
	mRawData[3] = static_cast<int>(t.packedUInt32());

	static bool timeInited = false;
	if (!timeInited) {
		timeInited = true;
		mLastUpdate = t;
	} else {
		const auto r0 = (mRawData[0] - mBias[0]) * mGyroScale;
		const auto r1 = (mRawData[1] - mBias[1]) * mGyroScale;
		const auto r2 = (mRawData[2] - mBias[2]) * mGyroScale;

		constexpr auto deltaConst = PI / 180 / 1000 / 1000000 / 2;
		const auto dt = static_cast<float>(t - mLastUpdate) * deltaConst;

		auto x = static_cast<float>(r0 * dt);
		auto y = static_cast<float>(r1 * dt);
		auto z = static_cast<float>(r2 * dt);

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
		mResult[0] = static_cast<int>(r0);
		mResult[1] = static_cast<int>(r1);
		mResult[2] = static_cast<int>(r2);
		mResult[3] = static_cast<int>(t.packedUInt32());

		mResult[4] = static_cast<int>(euler.x());
		mResult[5] = static_cast<int>(euler.z());
		mResult[6] = static_cast<int>(-euler.y());
		locker.unlock();

		Q_EMIT newData(mResult, t);
	}
}

void GyroSensor::countCalibrationParameters()
{
	disconnect(mIIOFile.data(), &trikHal::IIOFileInterface::newData, this, &GyroSensor::sumGyroscope);
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
	Q_EMIT calibrationFinished();
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
	mGyroSum[0] += gyroData[0];
	mGyroSum[1] += gyroData[1];
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
			roll = 0.0f;
			yaw = -std::atan2(-2.0f * (xy - zw), 1.0f - 2.0f * (yy + zz));
		}
	} else {
		roll = 0.0f;
		yaw = std::atan2(-2.0f * (xy - zw), 1.0f - 2.0f * (yy + zz));
	}

	pitch = pitch * RAD_TO_MDEG;
	yaw = yaw * RAD_TO_MDEG;
	roll = roll * RAD_TO_MDEG;
	return {pitch, roll, yaw};
}

void GyroSensor::onNewData(const QVector<int> &data, const trikKernel::TimeVal &eventTime)
{
	countTilt(data, eventTime);
}