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

#include "motorInterface.h"

namespace trikControl {

/// TRIK servomotor.
class ServoMotor : public MotorInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param min - minimal value of duty_ns whose meaning and range depends on motor type.
	/// @param max - maximal value of duty_ns whose meaning and range depends on motor type.
	/// @param zero - neutral value of duty_ns.
	/// @param stop - value of duty_ns corresponding to poweroff state.
	/// @param dutyFile - file for setting duty of PWM signal supplied to this motor.
	/// @param periodFile - file for setting period of PWM signal supplied to this motor.
	/// @param period - value of period for setting while initialization.
	/// @param invert - true, if power values set by setPower slot shall be negated before sent to motor.
	ServoMotor(int min, int max, int zero, int stop, QString const &dutyFile, QString const &periodFile, int period
			, bool invert);

public slots:
	/// Returns currently set power of continuous rotation servo or angle of angular servo.
	int power() const;

	/// Returns currently set frequency of PWM signal supplied to the motor.
	int frequency() const;

	/// Returns currently set duty of PWM signal supplied to the motor.
	int duty() const;

	/// Turns off motor. This is not the same as setPower(0), because setPower will
	/// leave motor on in a break mode, and this method will turn motor off.
	void powerOff();

protected:
	void setCurrentPower(int currentPower);
	void setCurrentDuty(int duty);
	void writeMotorCommand(QString const &command);
	int min() const;
	int max() const;
	int zero() const;
	bool invert() const;

private:
	QFile mDutyFile;
	QFile mPeriodFile;
	int const mPeriod;
	int mCurrentDutyPercent;
	int mMin;
	int mMax;
	int mZero;
	int mStop;
	bool mInvert;
	int mCurrentPower;
};

}
