#pragma once

#include <QtCore/QScopedPointer>
#include <QtCore/QThread>
#include <QQuaternion>
#include <QTimer>

#include <trikKernel/timeVal.h>

#include "gyroSensorInterface.h"
#include "deviceState.h"

namespace trikKernel {
class Configurer;
class TimeVal;
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
			, const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	~GyroSensor() override;

	Status status() const override;

public slots:
	QVector<int> read() const override;

	void calibrate(int msec) override;

	bool isCalibrated() const override;

private slots:

	void countTilt(QVector<int> gyroData, trikKernel::TimeVal t);

	void sumBias(QVector<int> gyroData, trikKernel::TimeVal);

	double getPitch(const QQuaternion &q) const;
	double getRoll(const QQuaternion &q) const;
	double getYaw(const QQuaternion &q) const;

	void initBias();

private:
	/// Device state, shared with worker.
	DeviceState mState;

	QScopedPointer<VectorSensorWorker> mVectorSensorWorker;
	QThread mWorkerThread;

	QTimer mCalibrationTimer;
	bool mIsCalibrated;

	QQuaternion mQ;

	QVector<int> mBias;
	QVector<int> mGyroSum;
	QVector<int> mResult;
	int mGyroCounter;

	trikKernel::TimeVal mLastUpdate;
};

}
