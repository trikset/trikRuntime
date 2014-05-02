/* Copyright 2013 Matvey Bryksin, Yurii Litvinov
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

#include "encoder.h"

#include "src/i2cCommunicator.h"

float const parToRad = 0.03272492;

using namespace trikControl;

Encoder::Encoder(I2cCommunicator &communicator, int i2cCommandNumber, QString const &port)
	: mCommunicator(communicator)
	, mPort(port)
	, mI2cCommandNumber(i2cCommandNumber)
{
}

void Encoder::reset()
{
	QByteArray command(2, '\0');
	command[0] = static_cast<char>(mI2cCommandNumber);
	command[1] = static_cast<char>(0x00);

	mCommunicator.send(command);
}

QString Encoder::portName() const
{
	return mPort;
}

float Encoder::read()
{
	QByteArray command(2, '\0');
	command[0] = static_cast<char>(mI2cCommandNumber);
	int data = mCommunicator.read(command);

	return parToRad * data;
}
