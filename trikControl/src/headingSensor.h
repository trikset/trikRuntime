#pragma once

#include <QtCore/QScopedPointer>
#include <QtCore/QThread>
#include <QQuaternion>
#include <QTimer>
#include <cmath>
#include <trikKernel/timeVal.h>

#include "headingSensorInterface.h"
#include "deviceState.h"
#include "gyroSensorInterface.h"
#include "vectorSensorInterface.h"

namespace trikControl {

class VectorSensorWorker;

class HeadingSensor : public HeadingSensorInterface
{
	Q_OBJECT

public:

	HeadingSensor(GyroSensorInterface *gyroscope, VectorSensorInterface *accelerometer);

	~HeadingSensor() override;

	Status status() const override;

public slots:

	QVector<int> read() const override;

	QVector<int> readRawData() const override;

	void calibrate(int msec) override;

	bool isCalibrated() const override;

private slots:

	void initAccelerometer();

	void sumAccelerometer(const QVector<int> &accelerometerData, const trikKernel::TimeVal &);

	void recountTilt(const QVector<int> &gyroData, const trikKernel::TimeVal &timestamp);

private:

	GyroSensorInterface *mGyroscope;

	VectorSensorInterface *mAccelerometer;

	bool mIsCalibrated;
	QTimer mCalibrationTimer;
	QVector<int> mAccelerometerVector;
	QVector<int> mAccelerometerSum;
	int mAccelerometerCounter;

	QVector<int> mResult;

};

}
