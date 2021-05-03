/* Copyright 2015 CyberTech Labs Ltd.
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

/// Communicates with MSP processor over USB bus.
class TRIKHAL_EXPORT MspUsbInterface
{
	Q_DISABLE_COPY(MspUsbInterface)
public:
	MspUsbInterface() = default;
	virtual ~MspUsbInterface() = default;

	/// Send data to MSP.
	virtual void send(const QByteArray &data) = 0;

	/// Reads data by given MSP command number and returns the result.
	virtual int read(const QByteArray &data) = 0;

	/// Establish connection with MSP over USB bus.
	virtual bool connect() = 0;

	/// Disconnect from MSP.
	virtual void disconnect() = 0;
};

}
