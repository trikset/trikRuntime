/* Copyright 2018 Aleksey Fefelov and CyberTech Labs Ltd.
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

#include "i2cdevice.h"

using namespace trikControl;

I2cDevice::I2cDevice(int bus, int adress
					 , trikControl::MspCommunicatorInterface &communicator)
	: mState("I2cDevice")
	, mCommunicator(communicator)


{
	mAdress = adress;
	mBus = bus;

	mState.ready();
}


I2cDevice::Status I2cDevice::status() const
{
	return combine(mCommunicator, mState.status());
}

void I2cDevice :: send(const QByteArray &data)
{
	mCommunicator.send(data);
}

int I2cDevice :: read(const QByteArray &data)
{
	return mCommunicator.read(data);
}
