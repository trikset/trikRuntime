/* Copyright 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include <QtCore/QByteArray>
#include <trikHal/trikHalDeclSpec.h>

namespace trikHal {

/// Communicates with MSP processor over I2C bus.
class TRIKHAL_EXPORT MspI2cInterface
{
	Q_DISABLE_COPY(MspI2cInterface)
public:
	MspI2cInterface() = default;

	virtual ~MspI2cInterface() = default;

	/// Send data to a device.
	virtual void send(const QByteArray &data) = 0;

	/// Reads data by given I2C command number and returns the result.
	virtual int read(const QByteArray &data) = 0;

	/// Establish connection with MSP over I2C bus.
	virtual bool connect(const QString &devicePath, int deviceId) = 0;

	/// Disconnect from MSP.
	virtual void disconnect() = 0;
};

}
