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

#include "vectorSensorInterface.h"

#include <trikControl/trikControlDeclSpec.h>

namespace trikControl {

/// Class that returns raw and processed data from the gyroscope.
class TRIKCONTROL_EXPORT GyroSensorInterface : public VectorSensorInterface
{
	Q_OBJECT

signals:

	/// Emites when average mean of bias is counted.
	void calibrationFinished();

public slots:
	/// Calibrates bias value of sensor, sets new bias value, resets other values include tilt values.
	/// Preffered duration of calibration -- 15-20 seconds.
	/// @param msec - duration of calibration in milliseconds
	virtual void calibrate(int msec) = 0;

	/// Returns vector with calibration values, which consists of 6 elements:
	/// 0-2 -- gyroscope bias, 3-5 -- average accelerometer values.
	/// Updated values is available only after calibration is done.
	virtual QVector<int> getCalibrationValues() = 0;

	/// Sets vector with calibration values to gyroscope sensor.
	/// It consists of 6 elements: 0-2 -- gyroscope bias, 3-5 -- average accelerometer values at rest.
	virtual void setCalibrationValues(const QVector<int> &values) = 0;

	/// Checks if bias is counted or not.
	virtual bool isCalibrated() const = 0;

	/// Returns current raw reading of a sensor.
	virtual QVector<int> readRawData() const = 0;
};

}

Q_DECLARE_METATYPE(trikControl::GyroSensorInterface *)
