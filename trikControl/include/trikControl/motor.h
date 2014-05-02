/* Copyright 2014 Roman Kurbatov
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

/// Abstract motor class. It is inherited by PowerMotor and ServoMotor classes.
class TRIKCONTROL_EXPORT Motor : public QObject
{
	Q_OBJECT

public:
	/// Destructor.
	virtual ~Motor() {}

	enum Type {
		powerMotor
		, servoMotor
	};

public slots:
	/// Sets current motor power to specified value, 0 to stop motor.
	/// @param power Power of a motor, from -100 (full reverse) to 100 (full forward), 0 --- break.
	virtual void setPower(int power) = 0;

	/// Returns currently set power of a motor.
	virtual int power() const = 0;

	/// Turns off motor. This is not the same as setPower(0), because setPower will
	/// leave motor on in a break mode, and this method will turn motor off.
	virtual void powerOff() = 0;

	/// Returns name of host port
	virtual QString portName() const = 0;
};

}
