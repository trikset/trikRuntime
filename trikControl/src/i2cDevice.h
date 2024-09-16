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

#include "i2cDeviceInterface.h"
#include "mspI2cCommunicator.h"
#include "i2cCommunicator.h"

namespace trikControl {

/// Abstract i2c device
class I2cDevice : public I2cDeviceInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param configurer - contains preparsed XML configuration.
	I2cDevice(const trikKernel::Configurer &configurer, trikHal::MspI2cInterface &i2c, int bus, int address);

	Status status() const override;

public slots:
	/// Sends data to current device, if it is connected.
	void send(int reg, int value) override;

	/// Reads data by given I2C command number and returns the result.
	int read(int reg) override;

private:
	DeviceState mState;
	I2cCommunicator mCommunicator;
};

}
