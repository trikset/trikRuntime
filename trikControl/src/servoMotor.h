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
#include <QtCore/QScopedPointer>

#include "motorInterface.h"

#include "deviceState.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
class OutputDeviceFileInterface;
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
	ServoMotor(const QString &port, const trikKernel::Configurer &configurer
			, const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	~ServoMotor() override;

	Status status() const override;

	int minControl() const override;

	int maxControl() const override;

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
	/// @param constrain - if true, power will be constrained in an interval [-100, 100], if false, "power" value
	///        will be sent to a motor unaltered.
	void setPower(int power, bool constrain = true) override;

	void forceBreak(int = 0) { /** Makes no sense, neither for angular servo nor for rotational */}

private:
	QScopedPointer<trikHal::OutputDeviceFileInterface> mDutyFile;
	QScopedPointer<trikHal::OutputDeviceFileInterface> mPeriodFile;
	QScopedPointer<trikHal::OutputDeviceFileInterface> mRunFile;
	int mPeriod;
	int mCurrentDutyPercent;
	int mMin;
	int mMax;
	int mZero;
	int mStop;
	int mMinControlRange;
	int mMaxControlRange;
	bool mInvert;
	int mCurrentPower;
	bool mRun;
	DeviceState mState;
};

}
