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

#include <QtCore/QString>
#include <QtCore/QMutex>

#include "deviceState.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class MspI2cInterface;
}

namespace trikControl {

/// Implementation of i2c communicator
class I2cCommunicator : public DeviceInterface
{
public:
	/// Constructor.
	/// @param i2c - hardware abstraction.
	/// @param bus - device bus (1 or 2)
	/// @param deviceId - device address on bus.
	I2cCommunicator(const trikKernel::Configurer &configurer, trikHal::MspI2cInterface &i2c
		, uint8_t bus, uint8_t deviceId);

	~I2cCommunicator();

	/// Send data to current device, if it is connected.
	void send(const QByteArray &data);

	/// Reads data by given I2C command number and returns the result.
	int read(const QByteArray &data);

	Status status() const override;

private:
	void disconnect();

	QMutex mLock;
	trikHal::MspI2cInterface &mI2c;
	DeviceState mState;
};

}
