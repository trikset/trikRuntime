/* Copyright 2026 CyberTech Labs Ltd.
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

/// Class that returns raw and calibrated data from a 3-axis magnetometer.
///
/// Inherits VectorSensorInterface, so read() returns a vector of three components
/// (x, y, z) of the measured magnetic field. Calibration compensates the two main
/// static distortions of the measurement:
///   - hard iron: a constant bias (permanent magnets, DC currents, sensor offset);
///   - soft iron: a linear distortion of the field (induced magnetization, scale
///     factor and non-orthogonality of the axes).
///
/// After calibration, read() returns corrected data whose magnitude is close to the
/// expected field magnitude F, while readRawData() always returns the uncorrected
/// (raw) reading. Calibration values can either be computed on-board by calibrate()
/// or supplied from an offline fit with setCalibrationValues().
class TRIKCONTROL_EXPORT MagnetometerSensorInterface : public VectorSensorInterface
{
	Q_OBJECT

public Q_SLOTS:
	/// Performs calibration (hard iron and soft iron) using readings collected during
	/// @param msec milliseconds. The device should be slowly rotated through all
	/// orientations while data is being collected, so that samples cover the whole sphere.
	virtual void calibrate(int msec) = 0;

	/// Sets calibration values computed elsewhere (for example, by an offline
	/// ellipsoid fit). Layout of @param values (12 elements, see getCalibrationValues()):
	///   [0..2]  hard iron bias b (x, y, z);
	///   [3..11] soft iron correction matrix A (3x3, row-major).
	virtual void setCalibrationValues(const QVector<double> &values) = 0;

public:
	/// Returns calibration values in the layout described in setCalibrationValues():
	/// 3 hard iron components and 9 soft iron matrix elements.
	Q_INVOKABLE virtual QVector<double> getCalibrationValues() = 0;

	/// Returns true if the sensor is calibrated, so read() returns corrected data.
	Q_INVOKABLE virtual bool isCalibrated() const = 0;

	/// Returns the current uncorrected (raw) reading of the sensor.
	Q_INVOKABLE virtual QVector<int> readRawData() const = 0;
};

}

Q_DECLARE_METATYPE(trikControl::MagnetometerSensorInterface *)
