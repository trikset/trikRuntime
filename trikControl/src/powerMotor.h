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

class I2cCommunicator;

/// TRIK power motor.
class PowerMotor : public MotorInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - port on which this motor is configured.
	/// @param configurer - configurer object containing preparsed XML files with motor parameters.
	/// @param communicator - I2C communicator to use to query sensor.
	PowerMotor(const QString &port, const trikKernel::Configurer &configurer, I2cCommunicator &communicator);

	~PowerMotor() override;

	Status status() const override;

public slots:
	/// Sets current motor power to specified value, 0 to stop motor.
	/// @param power Power of a motor, from -100 (full reverse) to 100 (full forward), 0 --- break.
	void setPower(int power);

	/// Returns currently set power of a motor.
	int power() const;

	/// Returns currently set period of a motor.
	int period() const;

	/// Turns off motor. This is not the same as setPower(0), because setPower will
	/// leave motor on in a break mode, and this method will turn motor off.
	void powerOff();

	/// Sets current motor period.
	/// Period of pulses is a time interval between two characteristic points of two adjacent pulses. 
	void setPeriod(int period);

private:
	I2cCommunicator &mCommunicator;
	int mI2cCommandNumber;
	const bool mInvert;
	int mCurrentPower;
	DeviceState mState;
	int mCurrentPeriod;
};

}
