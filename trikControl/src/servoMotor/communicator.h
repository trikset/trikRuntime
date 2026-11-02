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

#include "base.h"

namespace trikKernel {
class Configurer;
}

namespace trikControl {

class PeripheryDriverInterface;

/// Servo motor driven by the periphery module through the communicator.
class CommunicatorServoMotor final : public ServoMotorBase
{
public:
	CommunicatorServoMotor(const QString &port, const trikKernel::Configurer &configurer,
		PeripheryDriverInterface &driver);
	~CommunicatorServoMotor() override;

protected:
	void applyPeriod(uint32_t periodNs) override;
	void applyDuty(int pulseNs) override;
	void applyRun(bool on) override;
	void applyPowerOff() override;

private:
	void sendCommand(uint32_t periodNs, uint32_t pulseNs);

	PeripheryDriverInterface &mDriver;
};

} // namespace trikControl
