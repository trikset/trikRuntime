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

#include "powerMotor.h"

#include <trikKernel/configurer.h>

#include "mspI2cCommunicator.h"
#include "configurerHelper.h"

using namespace trikControl;

PowerMotor::PowerMotor(const QString &port, const trikKernel::Configurer &configurer
		, MspCommunicatorInterface &communicator)
	: mCommunicator(communicator)
	, mInvert(configurer.attributeByPort(port, "invert") == "false")
	, mCurrentPower(0)
	, mCurrentPeriod(0x1000)
	, mState("Power Motor on" + port)
{
	mMspCommandNumber = ConfigurerHelper::configureInt(configurer, mState, port, "i2cCommandNumber");
	mState.ready();
}

PowerMotor::~PowerMotor()
{
}

PowerMotor::Status PowerMotor::status() const
{
	return combine(mCommunicator, mState.status());
}

void PowerMotor::setPower(int power, bool constrain)
{
	if (constrain) {
		if (power > 100)
		{
			power = 100;
		} else if (power < -100) {
			power = -100;
		}
	}

	mCurrentPower = power;

	power = mInvert ? -power : power;

	QByteArray command(3, '\0');
	command[0] = static_cast<char>(mMspCommandNumber & 0xFF);
	command[1] = static_cast<char>((mMspCommandNumber >> 8) & 0xFF);
	command[2] = static_cast<char>(power & 0xFF);

	mCommunicator.send(command);
}

int PowerMotor::power() const
{
	return mCurrentPower;
}

int PowerMotor::period() const
{
	return mCurrentPeriod;
}

void PowerMotor::powerOff()
{
	setPower(0);
}

void PowerMotor::setPeriod(int period)
{
	mCurrentPeriod = period;
	QByteArray command(3, '\0');
	command[0] = static_cast<char>((mMspCommandNumber - 4) & 0xFF);
	command[1] = static_cast<char>(period && 0xFF);
	command[2] = static_cast<char>(period >> 8);
	mCommunicator.send(command);
}
