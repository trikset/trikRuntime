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

#include <QtCore/QScopedPointer>

#include "port/pwmChannelLease.h"
#include "servoBase.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
class OutputDeviceFileInterface;
}

namespace trikControl {

class PortManager;

/// Servo motor driven by the onboard PWM through sysfs files.
class InternalServoMotor final : public ServoMotorBase
{
public:
	InternalServoMotor(const QString &port, const trikKernel::Configurer &configurer,
		trikHal::HardwareAbstractionInterface &hardwareAbstraction, PortManager &ports);
	~InternalServoMotor() override;

protected:
	void applyPeriod(uint32_t periodNs) override;
	void applyDuty(int pulseNs) override;
	void applyRun(bool on) override;
	void applyPowerOff() override;

private:
	void failSetup();

	PwmChannelLease mPwmChannel;
	QScopedPointer<trikHal::OutputDeviceFileInterface> mDutyFile;
	QScopedPointer<trikHal::OutputDeviceFileInterface> mPeriodFile;
	QScopedPointer<trikHal::OutputDeviceFileInterface> mRunFile;
};

} // namespace trikControl
