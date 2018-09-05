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

#pragma once

#include "i2cdeviceinterface.h"
#include "mspI2cCommunicator.h"

namespace trikControl{

/// Abstract i2c device
class I2cDevice : public I2cDeviceInterface
{
	Q_OBJECT

public:
	I2cDevice(int bus, int adress
			  , trikControl::MspCommunicatorInterface &communicator);

	Status status() const override;

public slots:
	/// Send data to current device, if it is connected.
	void send(const QByteArray &data) override;

	/// Reads data by given I2C command number and returns the result.
	int read(const QByteArray &data) override;

private:
	int mBus;
	int mAdress;
	DeviceState mState;
	MspCommunicatorInterface &mCommunicator;
};

}
