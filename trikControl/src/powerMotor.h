/* Copyright 2013 - 2016 Yurii Litvinov, Anastasiia Kornilova and CyberTech Labs Ltd.
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
#include <QtCore/QVector>

#include "motorInterface.h"
#include "deviceState.h"

namespace trikKernel {
class Configurer;
}

namespace trikControl {

class MspCommunicatorInterface;

/// TRIK power motor.
class PowerMotor : public MotorInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - port on which this motor is configured.
	/// @param configurer - configurer object containing preparsed XML files with motor parameters.
	/// @param communicator - I2C communicator to use to query sensor.
	PowerMotor(const QString &port, const trikKernel::Configurer &configurer, MspCommunicatorInterface &communicator);

	~PowerMotor() override;

	Status status() const override;

	int minControl() const override;

	int maxControl() const override;

public slots:
	void setPower(int power, bool constrain = true) override;

	int power() const override;

	void powerOff() override;

	/// Returns currently set period of a motor.
	int period() const;

	/// Sets current motor period.
	/// Period of pulses is a time interval between two characteristic points of two adjacent pulses.
	void setPeriod(int period);

	/// Forced break for a short period of time followed by power off
	void forceBreak(int durationMs = 300);

private:
	void lineariseMotor(const QString &port, const trikKernel::Configurer &configurer);

	MspCommunicatorInterface &mCommunicator;
	int mMspCommandNumber;
	const bool mInvert;
	int mCurrentPower;
	int mCurrentPeriod;
	DeviceState mState;
	QVector<int> mPowerMap;
};

}
