#include "gyroSensor.h"

#include <trikKernel/configurer.h>
#include <trikKernel/timeVal.h>
#include <QsLog.h>
#include <cmath>

#include "vectorSensorWorker.h"

static const double mGyroInRad = 0.00875 * M_PI / 180;

using namespace trikControl;

GyroSensor::GyroSensor(const QString &deviceName, const trikKernel::Configurer &configurer
		, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mState(deviceName)
	, mIsCalibrated(true)
	, mQ(QQuaternion(1, 0, 0, 0))
	, mGyroCounter(0)
{
	mVectorSensorWorker.reset(new VectorSensorWorker(configurer.attributeByDevice(deviceName, "deviceFile"), mState
			, hardwareAbstraction, mWorkerThread));

	mBias << 0 << 0 << 0;
	mGyroSum << 0 << 0 << 0;

	mCalibrationTimer.moveToThread(&mWorkerThread);
	mCalibrationTimer.setSingleShot(true);

	if (!mState.isFailed()) {
		qRegisterMetaType<trikKernel::TimeVal>("trikKernel::TimeVal");
		connect(mVectorSensorWorker.data(), SIGNAL(newData(QVector<int>,trikKernel::TimeVal))
				, this, SLOT(countTilt(QVector<int>,trikKernel::TimeVal)));

		connect(mVectorSensorWorker.data(), SIGNAL(newData(QVector<int>,trikKernel::TimeVal))
				, this, SLOT(sumBias(QVector<int>,trikKernel::TimeVal)));

		connect(&mCalibrationTimer, SIGNAL(timeout()), this, SLOT(initBias()));

		QLOG_INFO() << "Starting VectorSensor worker thread" << &mWorkerThread;

		mState.ready();
	}
}

GyroSensor::~GyroSensor()
{
	if (mWorkerThread.isRunning()) {
		QMetaObject::invokeMethod(mVectorSensorWorker.data(), "deinitialize");
		mWorkerThread.quit();
		mWorkerThread.wait();
	}
}

GyroSensor::Status GyroSensor::status() const
{
	return mState.status();
}

QVector<int> GyroSensor::read() const
{
	QVector<int> gyro = mVectorSensorWorker->read();
	gyro[3] = getPitch(mQ) * 100;
	gyro[4] = getRoll(mQ) * 100;
	gyro[5] = getYaw(mQ) * 100;

	return gyro;
}

void GyroSensor::calibrate(int msec)
{
	mCalibrationTimer.start(msec);
	mIsCalibrated = false;
}

bool GyroSensor::isCalibrated() const
{
	return mIsCalibrated;
}

void GyroSensor::countTilt(QVector<int> gyroData, trikKernel::TimeVal t)
{
	static bool timeInited = false;
	if (!timeInited) {
		timeInited = true;
		mLastUpdate = t;
	} else {

		const auto dt = (t - mLastUpdate).toMcSec() / 1000000.0;

		const auto deltaConst = mGyroInRad * dt;

		const auto x = (gyroData[0] - mBias[0]) * deltaConst;
		const auto y = (gyroData[1] - mBias[1]) * deltaConst;
		const auto z = (gyroData[2] - mBias[2]) * deltaConst;

		const auto c1 = cos(x / 2);
		const auto s1 = sin(x / 2);
		const auto c2 = cos(y / 2);
		const auto s2 = sin(y / 2);
		const auto c3 = cos(z / 2);
		const auto s3 = sin(z / 2);

		QQuaternion deltaQ;
		deltaQ.setScalar(c1 * c2 * c3 + s1 * s2 * s3);
		deltaQ.setX(s1 * c2 * c3 - c1 * s2 * s3);
		deltaQ.setY(c1 * s2 * c3 + s1 * c2 * s3);
		deltaQ.setZ(c1 * c2 * s3 - s1 * s2 * c3);

		mQ *= deltaQ;
		mQ.normalize();

		mLastUpdate = t;
	}
}

void GyroSensor::sumBias(QVector<int> gyroData, trikKernel::TimeVal)
{
	if (!mIsCalibrated) {
		mGyroSum[0] += gyroData[0];
		mGyroSum[1] += gyroData[1];
		mGyroSum[2] += gyroData[2];

		mGyroCounter++;
	}
}

double GyroSensor::getPitch(const QQuaternion &q) const
{
	return atan2(2 * q.y()*q.z() + 2 * q.scalar() * q.x()
				 , 1 - 2 * q.x() * q.x() - 2 * q.y() * q.y());
}

double GyroSensor::getRoll(const QQuaternion &q) const
{
	return asin(2 * q.scalar() * q.y() - 2 * q.x() * q.y());
}

double GyroSensor::getYaw(const QQuaternion &q) const
{
	return atan2(2 * q.x() * q.y() + 2 * q.scalar() * q.z()
				 , 1 - 2 * q.y() * q.y() - 2 * q.z() * q.z());
}

void GyroSensor::initBias()
{
	if (mGyroCounter != 0) {
		for (int i = 0; i <= 2; i++) {
			mBias[i] = mGyroSum[i] / mGyroCounter;
			qDebug() << mBias[i];
			mGyroSum[i] = 0;
		}

		mGyroCounter = 0;
		mIsCalibrated = true;
		mQ = QQuaternion(1, 0, 0, 0);
	}
}
