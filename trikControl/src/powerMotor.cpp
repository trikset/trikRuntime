/* Copyright 2013 Yurii Litvinov
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

#include <QtCore/QDebug>

#include "i2cCommunicator.h"

using namespace trikControl;

PowerMotor::PowerMotor(I2cCommunicator &communicator, int i2cCommandNumber, bool invert, QString const &port)
	: mCommunicator(communicator)
	, mPort(port)
	, mI2cCommandNumber(i2cCommandNumber)
	, mInvert(invert)
	, mCurrentPower(0)
{
}

PowerMotor::~PowerMotor()
{
	powerOff();
}

void PowerMotor::setPower(int power)
{
	if (power > 100) {
		power = 100;
	} else if (power < -100) {
		power = -100;
	}

	mCurrentPower = power;

	power = mInvert ? -power : power;

	QByteArray command(2, '\0');
	command[0] = static_cast<char>(mI2cCommandNumber & 0xFF);
	command[1] = static_cast<char>(power & 0xFF);

	mCommunicator.send(command);
}

int PowerMotor::power() const
{
	return mCurrentPower;
}

void PowerMotor::powerOff()
{
	setPower(0);
}

QString PowerMotor::portName() const
{
	return mPort;
}
