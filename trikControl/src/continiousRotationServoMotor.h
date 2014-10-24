/* Copyright 2014 CyberTech Labs Ltd.
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

#include "servoMotor.h"

namespace trikControl {

/// Continious rotation servomotor.
class ContiniousRotationServoMotor : public ServoMotor
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param min - value of duty_ns corresponding to full reverse of a motor. Used to calculate actual values from
	///        values in range [-100..100] from client program.
	/// @param max - value of duty_ns corresponding to full forward of a motor. Used to calculate actual values from
	///        values in range [-100..100] from client program.
	/// @param zero - value of duty_ns corresponding to full stop of a motor.
	/// @param stop - value of duty_ns corresponding to poweroff state.
	/// @param dutyFile - file for setting duty of PWM signal supplied to this motor.
	/// @param periodFile - file for setting period of PWM signal supplied to this motor
	/// @param period - value of period for setting while initialization
	/// @param invert - true, if power values set by setPower slot shall be negated before sent to motor.
	ContiniousRotationServoMotor(int min, int max, int zero, int stop, QString const &dutyFile
			, QString const &periodFile, int period, bool invert);

public slots:
	/// Sets current motor power to specified value, 0 to stop motor.
	/// @param power - power of the motor, from -100 (full reverse) to 100 (full forward), 0 --- break.
	void setPower(int power);
};

}
