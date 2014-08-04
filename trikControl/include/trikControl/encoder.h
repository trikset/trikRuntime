/* Copyright 2013 Matvey Bryksin, Yurii Litvinov
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

#include "declSpec.h"

namespace trikControl {

class I2cCommunicator;

/// Encoder of power motor.
class TRIKCONTROL_EXPORT Encoder : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param communicator - I2C communicator.
	/// @param i2cCommandNumber - number of I2C command to query this encoder.
	/// @param rawToDegrees - coefficient for converting raw encoder readings to degrees.
	Encoder(I2cCommunicator &communicator, int i2cCommandNumber, double rawToDegrees);

public slots:
	/// Returns current encoder reading (in degrees).
	float read();

	/// Resets encoder by setting current reading to 0.
	void reset();

private:
	I2cCommunicator &mCommunicator;
	int mI2cCommandNumber;
	double mRawToDegrees;
};

}
