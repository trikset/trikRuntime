/* Copyright 2013 - 2026 Yurii Litvinov, Anastasiia Kornilova and CyberTech Labs Ltd.
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

#include <memory>

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QVector>

#include "motorInterface.h"
#include "deviceState.h"
#include "port/gpioLineLease.h"
#include "port/portLease.h"
#include "port/pwmChannelLease.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
class InputDeviceFileInterface;
class OutputDeviceFileInterface;
}

namespace trikControl {

class PortManager;

static const int powerMotorMaxControl = 100;
static const int powerMotorMinControl = -100;

/// Power motor driven by on-board hardware through sysfs.
class PowerMotor : public MotorInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - port on which this motor is configured.
	/// @param configurer - configurer object containing preparsed XML files with motor parameters.
	/// @param hw - hardware abstraction layer implementation.
	PowerMotor(const QString &port, const trikKernel::Configurer &configurer,
		trikHal::HardwareAbstractionInterface &hw, PortManager &ports);

	~PowerMotor() override;

	Status status() const override;
	int minControl() const override;
	int maxControl() const override;

public Q_SLOTS:
	// NOLINTNEXTLINE(google-default-arguments)
	void setPower(int power, bool constrain = true) override;

	int power() const override;

	void powerOff() override;

	void setPeriod(int uSec) override;

	/// Force brake for a short period of time followed by power off.
	// NOLINTNEXTLINE(google-default-arguments)
	void brake(int durationMs = 500) override;

	/// Returns the latest raw SARADC value of the motor current-sense channel.
	int readRawSense();

	/// Returns the latest motor current-sense voltage in volts.
	float readSense();

	/// Returns true when the driver reports an active-low fault.
	bool hasFault() const;

private:
	void lineariseMotor(const QString &port, const trikKernel::Configurer &configurer);

	PortLease mPortLease;
	PwmChannelLease mPwmChannel;
	GpioLineLease mDirLine;
	GpioLineLease mFaultLine;
	std::unique_ptr<trikHal::OutputDeviceFileInterface> mPeriodFile;
	std::unique_ptr<trikHal::OutputDeviceFileInterface> mDutyFile;
	std::unique_ptr<trikHal::OutputDeviceFileInterface> mRunFile;
	std::unique_ptr<trikHal::OutputDeviceFileInterface> mDirFile;

	std::unique_ptr<trikHal::InputDeviceFileInterface> mSenseFile;
	std::unique_ptr<trikHal::InputDeviceFileInterface> mFaultFile;

	int mCurrentPower = 0;
	int mPeriodNs = 50000;
	bool mInvert = false;
	double mSenseScaleMv = 1.7578125;
	DeviceState mState;
	QVector<int> mPowerMap;
};

}

// NOLINTNEXTLINE(misc-no-recursion)
Q_DECLARE_METATYPE(trikControl::PowerMotor *)
