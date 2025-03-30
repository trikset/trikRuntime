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

namespace trikHal {
class MspI2cInterface;
}

namespace trikControl{

/// Abstract i2c device
class I2cDevice : public I2cDeviceInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param configurer - contains preparsed XML configuration.
	/// Takes ownership of i2c
	I2cDevice(const trikKernel::Configurer &configurer, trikHal::MspI2cInterface *i2c, int bus, int address);
	~I2cDevice();
	Status status() const override;

public Q_SLOTS:
	/// Sends byte data to current device, if it is connected.
	void send8(uint16_t reg, uint8_t value) override;

	/// Sends word data to current device, if it is connected.
	void send16(uint16_t reg, uint16_t value) override;

	/// Reads byte by given I2C command number and returns the result.
	int read8(uint16_t reg) override;

	/// Reads word by given I2C command number and returns the result.
	int read16(uint16_t reg) override;

	/// Reads 4 bytes by given I2C command number and returns the result.
	int read32(uint16_t reg) override;

private:
	int read(uint16_t reg, uint16_t number);
	DeviceState mState;
	QScopedPointer<trikHal::MspI2cInterface> mInterface;
	QScopedPointer<MspCommunicatorInterface> mCommunicator;
};

}
