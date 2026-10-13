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

#include "mspDriver.h"

#include "mspI2cCommunicator.h"
#include "mspUsbCommunicator.h"

using namespace trikControl;

namespace {

/// Builds the wire payload expected by trikHal::TrikI2c:
///   [register(1B), 0x00, command.data...]
/// For writes data[2..3] carry the value (byte/word), for reads data[2..3]
/// carry the SMBus read size. Byte [1] is ignored by the I2C backend.
QByteArray toI2cPayload(const CommunicatorCommand &command)
{
	QByteArray payload(2 + command.data.size(), '\0');
	payload[0] = static_cast<char>(command.device);
	payload.replace(2, command.data.size(), command.data);
	return payload;
}

} // namespace

MspDriver::MspDriver(MspUsbCommunicator *usb, MspI2cCommunicator *i2c)
	: mUsb(usb)
	, mI2c(i2c)
{
}

MspDriver::~MspDriver() = default;

void MspDriver::send(const CommunicatorCommand &command)
{
	if (mI2c) {
		mI2c->send(toI2cPayload(command));
	}
}

void MspDriver::sendBatch(const CommunicatorCommand &command)
{
	Q_UNUSED(command)
}

int MspDriver::read(const CommunicatorCommand &command)
{
	return mI2c ? mI2c->read(toI2cPayload(command)) : 0;
}

int MspDriver::readAsync(const QString &port)
{
	Q_UNUSED(port)
	return 0;
}

DeviceInterface::Status MspDriver::status() const
{
	if (mI2c) {
		return mI2c->status();
	}

	if (mUsb) {
		return mUsb->status();
	}

	return DeviceInterface::Status::off;
}