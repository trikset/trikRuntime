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

#include <QtCore/QByteArray>

#include <cstdint>

namespace trikControl {

/// Universal structured command for communicating with a periphery module.
///
/// Regmap-style access to the periphery module (like the Linux regmap API): a
/// command addresses a hardware block (device) and performs an operation on a
/// port of that block. The concrete driver is free to pick the protocol and
/// transport (USB, I2C, ...) and to serialize the command differently, so the
/// client does not depend on the wire format.
struct CommunicatorCommand {
	/// Logical name of the subsystem / hardware block on the periphery module
	/// (e.g. PWM generator, control block, port block), or a raw command
	/// register number for transports that use one (like MSP I2C).
	uint8_t device;

	/// Port (connector) name within the subsystem. Unused by transports that
	/// address a single register, reserved for future use.
	uint8_t address;

	/// Operation to perform on the port (read value, set value, reset, ...).
	/// Unused by transports that imply read/write by the called method,
	/// reserved for future use.
	uint8_t operation;

	/// Command payload (e.g. power value, period and pulse, read size).
	QByteArray data;

	/// Serialize to a flat byte array: [device, address, operation, data...].
	QByteArray flatten() const
	{
		QByteArray out(3 + data.size(), '\0');
		out[0] = device;
		out[1] = address;
		out[2] = operation;
		out.replace(3, data.size(), data);
		return out;
	}
};

} // namespace trikControl