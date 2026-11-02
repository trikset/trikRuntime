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

#include "usbDeviceInterface.h"

namespace trikHal {

/// Communicates with the MSP processor over USB. Kept as a concrete flavor of
/// the common USB transport interface.
class TRIKHAL_EXPORT MspUsbInterface : public UsbDeviceInterface
{
	Q_DISABLE_COPY(MspUsbInterface)
public:
	MspUsbInterface() = default;
	~MspUsbInterface() override = default;

	/// Sends one command to MSP.
	int send(const QByteArray &data) override = 0;

	/// Reads a value for the given MSP command.
	int read(const QByteArray &data) override = 0;

	/// Establishes connection with MSP over USB.
	bool connect() override = 0;

	/// Disconnects from MSP.
	void disconnect() override = 0;
};

}
