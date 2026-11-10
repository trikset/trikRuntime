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

#include "servoCommunicator.h"

#include <QtCore/QByteArray>

#include <trikKernel/configurer.h>

#include "communicatorCommand.h"
#include "peripheryDriverInterface.h"

using namespace trikControl;

CommunicatorServoMotor::CommunicatorServoMotor(const QString &port, const trikKernel::Configurer &configurer,
	PeripheryDriverInterface &driver, PortManager &ports)
	: ServoMotorBase(port, configurer, ports)
	, mDriver(driver)
{
	if (mState.isFailed()) {
		return;
	}

	markReady();
}

CommunicatorServoMotor::~CommunicatorServoMotor() = default;

void CommunicatorServoMotor::applyPeriod(uint32_t)
{
}

void CommunicatorServoMotor::applyDuty(int pulseNs)
{
	sendCommand(static_cast<uint32_t>(mPeriod), static_cast<uint32_t>(pulseNs));
}

void CommunicatorServoMotor::applyRun(bool)
{
}

void CommunicatorServoMotor::applyPowerOff()
{
	sendCommand(static_cast<uint32_t>(mPeriod), 0);
}

void CommunicatorServoMotor::sendCommand(uint32_t periodNs, uint32_t pulseNs)
{
	QByteArray data(8, '\0');
	for (int i = 0; i < 4; ++i) {
		data[i] = static_cast<char>((periodNs >> (8 * i)) & 0xFF);
		data[4 + i] = static_cast<char>((pulseNs >> (8 * i)) & 0xFF);
	}

	const CommunicatorCommand command {
		communicatorCommand::kDevicePort,
		port(),
		communicatorCommand::kPortSetValue,
		data,
	};

	mDriver.send(command);
}
