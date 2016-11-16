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
	void biasInited();

public slots:
	virtual void calibrate(int msec) = 0;

	virtual bool isCalibrated() const = 0;
};

}
