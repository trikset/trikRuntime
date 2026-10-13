/* Copyright 2026 CyberTech Labs Ltd.
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

#include "peripheryDriverInterface.h"

namespace trikControl {

class MspUsbCommunicator;
class MspI2cCommunicator;

/// Concrete periphery module driver for MSP.
///
/// Owns nothing: both communicators are created by the caller and outlive the
/// driver. Routes requests to USB or I2C according to its own logic.
class MspDriver : public PeripheryDriverInterface
{
public:
	/// @param usb - USB communicator, may be nullptr.
	/// @param i2c - I2C communicator, may be nullptr.
	MspDriver(MspUsbCommunicator *usb, MspI2cCommunicator *i2c);

	~MspDriver() override;

	void send(const CommunicatorCommand &command) override;
	void sendBatch(const CommunicatorCommand &command) override;
	int read(const CommunicatorCommand &command) override;
	int readAsync(const QString &port) override;

	Status status() const override;

private:
	MspUsbCommunicator *mUsb;
	MspI2cCommunicator *mI2c;
};

} // namespace trikControl