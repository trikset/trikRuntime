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

/// The current implementation of SMBus MspI2c is suitable for interacting
/// with many devices, so it was decided to extend this interface from trikHal.
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
	/// @param i2c - interface for i2c communication
	/// @param bus - bus for i2c communication
	/// @param address - address for device on bus
	/// Takes ownership of i2c
	I2cDevice(const trikKernel::Configurer &configurer, trikHal::MspI2cInterface *i2c, int bus, int address);

	/// For implicit forward declaration
	~I2cDevice();

	Status status() const override;
public Q_SLOTS:

	/// Sends byte/word data to current device, if it is connected.
	int send(int reg, int value, const QString &mode = "b") override;

	/// Reads byte/word data by given I2C command number and returns the result.
	int read(int reg, const QString &mode = "b") override;

	/// Reads data by given I2C command number and returns the result.
	QVector<uint8_t> readX(int reg, int size) override;
private:
	DeviceState mState;
	QScopedPointer<trikHal::MspI2cInterface> mInterface;
	QScopedPointer<MspCommunicatorInterface> mCommunicator;
};

}
