/* Copyright 2013 - 2015 Yurii Litvinov and CyberTech Labs Ltd.
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
#include <QtCore/QByteArray>
#include <QtCore/QMutex>

#include "deviceInterface.h"
#include "deviceState.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class I2CInterface;
}

namespace trikControl {

/// Provides direct interaction with I2C device.
/// @todo: It shall work in separate thread.
class I2cCommunicator : public DeviceInterface
{
public:
	/// Constructor.
	/// @param configurer - contains preparsed XML configuration.
	I2cCommunicator(const trikKernel::Configurer &configurer, trikHal::I2CInterface &i2c);

	~I2cCommunicator();

	/// Send data to current device, if it is connected.
	void send(const QByteArray &data);

	/// Reads data by given I2C command number and returns the result.
	int read(const QByteArray &data);

	Status status() const override;

private:
	void disconnect();

	QMutex mLock;
	DeviceState mState;
	trikHal::I2CInterface &mI2c;
};

}
