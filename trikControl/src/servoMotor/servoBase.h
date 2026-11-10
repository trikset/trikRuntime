/* Copyright 2026 CyberTech Labs Ltd.
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

#include <cstdint>

#include <QtCore/QString>

#include "deviceState.h"
#include "port/portLease.h"
#include "motorInterface.h"

namespace trikKernel {
class Configurer;
}

namespace trikControl {

class PortManager;

/// Common base for servo motor implementations.
///
/// Servo motors can be served either by the onboard hardware abstraction or by
/// the periphery module. Derived classes only implement the transport-specific
/// operations, while the control logic lives here.
class ServoMotorBase : public MotorInterface
{
public:
	ServoMotorBase(const QString &port, const trikKernel::Configurer &configurer, PortManager &ports);
	~ServoMotorBase() override;

	Status status() const override;

	int minControl() const override;

	int maxControl() const override;

	/// Returns currently set frequency of PWM signal supplied to the motor.
	int frequency() const;

	/// Returns currently set duty of PWM signal supplied to the motor.
	int duty() const;

public Q_SLOTS:
	int power() const override;

	// NOLINTNEXTLINE(google-default-arguments)
	void setPower(int power, bool constrain = true) override;

	void powerOff() override;

	// NOLINTNEXTLINE(google-default-arguments)
	void brake(int /* durationMs */ = 0) override
	{ /** Makes no sense, neither for angular servo nor for rotational */
	}

	// NOLINTNEXTLINE(google-default-arguments)
	void setPeriod(int uSec = 20000) override;

protected:
	const QString &port() const;

	/// Marks the device as ready after the derived implementation has
	/// initialized its transport.
	void markReady();

	virtual void applyPeriod(uint32_t periodNs) = 0;
	virtual void applyDuty(int pulseNs) = 0;
	virtual void applyRun(bool on) = 0;
	virtual void applyPowerOff() = 0;

	PortLease mPortLease;
	int mPeriod = 0;
	int mMin = 0;
	int mMax = 0;
	int mZero = 0;
	int mStop = 0;
	int mMinControlRange = 0;
	int mMaxControlRange = 0;
	int mCurrentDutyPercent = 0;
	int mCurrentPower = 0;
	bool mInvert = false;
	bool mRun = false;
	DeviceState mState;
	const QString mPortName;
};

} // namespace trikControl
