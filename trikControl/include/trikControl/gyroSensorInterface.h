#pragma once

#include <QtCore/QObject>
#include <QtCore/QVector>

#include "vectorSensorInterface.h"

#include "declSpec.h"

namespace trikKernel {
class TimeVal;
}

namespace trikControl {

class TRIKCONTROL_EXPORT GyroSensorInterface : public VectorSensorInterface
{
	Q_OBJECT

signals:

	///Emites when average mean of bias is counted
	void biasInited();

public slots:
	///Calibrates bias value of sensor, sets new bias value, resets other values include tilt values
	/// @param msec - duration of calibration in milliseconds
	virtual void calibrate(int msec) = 0;

	///Checks if bias is counted or not
	virtual bool isCalibrated() const = 0;

	/// Returns current raw reading of a sensor.
	virtual QVector<int> readRawData() const = 0;
};

}
