/* Copyright 2013 - 2015 Matvey Bryksin, Yurii Litvinov and CyberTech Labs Ltd.
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

#include "battery.h"

#include "mspI2cCommunicator.h"

using namespace trikControl;

Battery::Battery(MspCommunicatorInterface &communicator)
	: mCommunicator(communicator)
{
}

float Battery::readVoltage()
{
	/// @todo: Remove these arcane numbers, or Something may be unexpectedly summoned by them.
	return (readRawDataVoltage() / 1023.0) * 3.3 * (7.15 + 2.37) / 2.37;
}

float Battery::readRawDataVoltage()
{
	QByteArray command(4, '\0');
	command[0] = static_cast<char>(0x26);
	command[1] = static_cast<char>(0x00);

	command[2] = static_cast<char>(0x02);
	command[3] = static_cast<char>(0x00);

	return mCommunicator.read(command);
}

Battery::Status Battery::status() const
{
	return mCommunicator.status();
}
