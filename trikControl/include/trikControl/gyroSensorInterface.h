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
class TRIKCONTROL_EXPORT GyroSensorInterface : public VectorSensorInterface
{
	Q_OBJECT

signals:

	/// Emites when average mean of bias is counted.
	void biasInited();

public slots:
	/// Calibrates bias value of sensor, sets new bias value, resets other values include tilt values.
	/// Preffered duration of calibration -- 15-20 seconds.
	/// @param msec - duration of calibration in milliseconds
	virtual void calibrate(int msec) = 0;

	/// Checks if bias is counted or not.
	virtual bool isCalibrated() const = 0;

	/// Returns current raw reading of a sensor.
	virtual QVector<int> readRawData() const = 0;
};

}
