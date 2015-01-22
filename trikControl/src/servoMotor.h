/* Copyright 2013 - 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include "deviceState.h"

namespace trikKernel {
class Configurer;
}

namespace trikControl {

/// TRIK servomotor.
class ServoMotor : public MotorInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - port on which this motor is configured.
	/// @param configurer - configurer object containing preparsed XML files with motor parameters.
	ServoMotor(QString const &port, trikKernel::Configurer const &configurer);

	Status status() const override;

public slots:
	int power() const override;

	/// Returns currently set frequency of PWM signal supplied to the motor.
	int frequency() const;

	/// Returns currently set duty of PWM signal supplied to the motor.
	int duty() const;

	void powerOff() override;

	/// For angular servos sets current motor angle to specified value. For continious rotation servos sets power of
	/// a motor.
	/// @param power - for angular servos --- servo shaft angle, allowed values are from -90 to 90, for continious
	///        rotation servos --- power, allowed values are from -100 to 100.
	void setPower(int power) override;

private:
	enum class Type {
		angular
		, continiousRotation
	};

	QFile mDutyFile;
	QFile mPeriodFile;
	int mPeriod;
	int mCurrentDutyPercent;
	int mMin;
	int mMax;
	int mZero;
	int mStop;
	bool mInvert;
	int mCurrentPower;
	DeviceState mState;
	Type mMotorType;
};

}
