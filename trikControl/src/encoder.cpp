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

#include "encoder.h"

#include <trikKernel/configurer.h>

#include "i2cCommunicator.h"
#include "configurerHelper.h"

using namespace trikControl;

Encoder::Encoder(QString const &port, trikKernel::Configurer const &configurer, I2cCommunicator &communicator)
	: mCommunicator(communicator)
{
	mI2cCommandNumber = ConfigurerHelper::configureInt(configurer, mState, port, "i2cCommandNumber");
	mRawToDegrees = ConfigurerHelper::configureReal(configurer, mState, port, "rawToDegrees");

	mState.ready();
}

void Encoder::reset()
{
	if (status() == DeviceInterface::Status::ready) {
		QByteArray command(2, '\0');
		command[0] = static_cast<char>(mI2cCommandNumber);
		command[1] = static_cast<char>(0x00);

		mCommunicator.send(command);
	}
}

Encoder::Status Encoder::status() const
{
	return combine(mCommunicator, mState.status());
}

int Encoder::read()
{
	if (status() == DeviceInterface::Status::ready) {
		QByteArray command(2, '\0');
		command[0] = static_cast<char>(mI2cCommandNumber);
		int data = mCommunicator.read(command);

		return mRawToDegrees * data;
	} else {
		return 0;
	}
}

int Encoder::readRawData()
{
	if (status() == DeviceInterface::Status::ready) {
		QByteArray command(2, '\0');
		command[0] = static_cast<char>(mI2cCommandNumber);

		return mCommunicator.read(command);
	} else {
		return 0;
	}
}
