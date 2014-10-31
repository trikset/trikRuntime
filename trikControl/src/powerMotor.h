/* Copyright 2013 Yurii Litvinov
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
#include <QtCore/QString>
#include <QtCore/QFile>

#include "motor.h"

namespace trikControl {

class I2cCommunicator;

/// TRIK power motor.
class PowerMotor : public Motor
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param communicator - reference to an object that handles I2C communication.
	/// @param i2cCommandNumber - I2C command corresponding to this device.
	/// @param invert - true, if power values set by setPower slot shall be negated before sent to motor.
	PowerMotor(I2cCommunicator &communicator, int i2cCommandNumber, bool invert);

	/// Destructor.
	~PowerMotor() override;

public slots:
	/// Sets current motor power to specified value, 0 to stop motor.
	/// @param power Power of a motor, from -100 (full reverse) to 100 (full forward), 0 --- break.
	void setPower(int power);

	/// Returns currently set power of a motor.
	int power() const;

	/// Turns off motor. This is not the same as setPower(0), because setPower will
	/// leave motor on in a break mode, and this method will turn motor off.
	void powerOff();

private:
	I2cCommunicator &mCommunicator;
	int const mI2cCommandNumber;
	bool const mInvert;
	int mCurrentPower;
};

}
