#pragma once

#include <QtCore/QObject>
#include <QtCore/QVector>

#include <trikKernel/timeVal.h>
#include "vectorSensorInterface.h"

#include "declSpec.h"
namespace trikKernel {
class TimeVal;
}

namespace trikControl {

/// Class that returns raw and processed data from the gyroscope.
class TRIKCONTROL_EXPORT HeadingSensorInterface : public VectorSensorInterface
{
	Q_OBJECT

signals:

	void inited();

public slots:

	virtual void calibrate(int msec) = 0;

	virtual bool isCalibrated() const = 0;

	virtual QVector<int> readRawData() const = 0;
};

}
