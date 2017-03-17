/* Copyright 2015 CyberTech Labs Ltd.
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

#include "deviceInterface.h"

#include "declSpec.h"

namespace trikControl {

/// Abstract motor. It is differently implemented by different motor types (like servo and power motors).
class TRIKCONTROL_EXPORT MotorInterface : public QObject, public DeviceInterface
{
	Q_OBJECT

public:
	enum class Type {
		powerMotor
		, servoMotor
	};

	/// Returns minimal value of control accepted by a motor (-90..90, 0..100, for example).
	virtual int minControl() const = 0;

	/// Returns maximal value of control accepted by a motor (-90..90, 0..100, for example).
	virtual int maxControl() const = 0;

public slots:
	/// Sets current motor power to specified value, 0 to stop motor.
	/// @param power - power of a motor, from -100 (full reverse) to 100 (full forward), 0 --- break.
	/// @param constrain - if true, power will be constrained in an interval [-100, 100], if false, "power" value
	///        will be sent to a motor unaltered.
	virtual void setPower(int power, bool constrain = true) = 0;

	/// Returns currently set power of a motor.
	virtual int power() const = 0;

	/// Turns off motor. This is not the same as setPower(0), because setPower can
	/// leave motor on in a break mode, and this method will turn motor off.
	virtual void powerOff() = 0;

	/// Force break state for durationMs and then powerOff()
	virtual void forceBreak(int durationMs = 300) = 0;
};

}
