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

#include "declSpec.h"

namespace trikControl {

/// Generic TRIK servomotor.
class TRIKCONTROL_EXPORT ServoMotor : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param controlFile - device file for this motor.
	/// @param min - value of duty_ns corresponding to full reverse of a motor. Used to calculate actual values from
	///        values in range [-100..100] from client program.
	/// @param max - value of duty_ns corresponding to full forward of a motor. Used to calculate actual values from
	///        values in range [-100..100] from client program.
	/// @param zero - value of duty_ns corresponding to full stop of a motor.
	/// @param deviceFile - device file for this motor.
	/// @param invert - true, if power values set by setPower slot shall be negated before sent to motor.
	ServoMotor(int min, int max, int zero, int stop, QString const& deviceFile, bool invert);

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
	QFile mControlFile;
	int mMin;
	int mMax;
	int mZero;
	int mStop;
	bool mInvert;
	int mCurrentPower;
};

}
