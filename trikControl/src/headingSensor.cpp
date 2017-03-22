#include "headingSensor.h"

#include <QDebug>

using namespace trikControl;

HeadingSensor::HeadingSensor(GyroSensorInterface *gyroscope, VectorSensorInterface *accelerometer):
	mGyroscope(gyroscope)
  , mAccelerometer(accelerometer)
{
	mCalibrationTimer.setSingleShot(true);

	mAccelerometerVector.resize(3);
	mAccelerometerSum.resize(3);
	mAccelerometerCounter = 0;

	mResult.resize(7);

	connect(&mCalibrationTimer, SIGNAL(timeout()), this, SLOT(initAccelerometer()));
	connect(mGyroscope, SIGNAL(newData(QVector<int>,trikKernel::TimeVal))
			, this, SLOT(recountTilt(QVector<int>,trikKernel::TimeVal)));
}

HeadingSensor::~HeadingSensor()
{}

HeadingSensor::Status HeadingSensor::status() const
{
	return HeadingSensor::Status::ready;
}

QVector<int> HeadingSensor::read() const
{
	return QVector<int>();
}

QVector<int> HeadingSensor::readRawData() const
{
	return QVector<int>();
}

void HeadingSensor::calibrate(int msec)
{
	mGyroscope->calibrate(msec);

	connect(mAccelerometer, SIGNAL(newData(QVector<int>,trikKernel::TimeVal))
			, this, SLOT(sumAccelerometer(QVector<int>,trikKernel::TimeVal)));

	mCalibrationTimer.start(msec);
	mIsCalibrated = false;
}

bool HeadingSensor::isCalibrated() const
{
	return (mIsCalibrated && mGyroscope->isCalibrated());
}

void HeadingSensor::initAccelerometer()
{
	disconnect(mAccelerometer, SIGNAL(newData(QVector<int>,trikKernel::TimeVal))
			, this, SLOT(sumAccelerometer(QVector<int>,trikKernel::TimeVal)));

	if (mAccelerometerCounter != 0) {
		for (int i = 0; i < 3; i++) {
			mAccelerometerVector[i] = mAccelerometerSum[i] / mAccelerometerCounter;
			qDebug() << mAccelerometerVector[i];
			mAccelerometerSum[i] = 0;
		}
	}

	mAccelerometerCounter = 0;
	mIsCalibrated = true;
}

void HeadingSensor::sumAccelerometer(const QVector<int> &accelerometerData, const trikKernel::TimeVal &)
{
	mAccelerometerSum[0] += accelerometerData[0];
	mAccelerometerSum[1] += accelerometerData[1];
	mAccelerometerSum[2] += accelerometerData[2];

	mAccelerometerCounter++;
}

void HeadingSensor::recountTilt(const QVector<int> &gyroData, const trikKernel::TimeVal &timestamp)
{
	mResult = gyroData;
	emit newData(mResult, timestamp);
}
