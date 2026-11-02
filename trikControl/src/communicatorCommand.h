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
#include <QtCore/QString>

#include <cstdint>

namespace trikControl {

/// Transport-agnostic constants for periphery module commands.
namespace communicatorCommand {

/// device ids
constexpr uint8_t kDeviceLoopback = 0x00;
constexpr uint8_t kDevicePwm = 0x03;
constexpr uint8_t kDeviceControl = 0x06;
constexpr uint8_t kDevicePort = 0x07;

/// port ids for kDevicePort
constexpr uint8_t kPortGetValue = 0x01;
constexpr uint8_t kPortSetValue = 0x02;
constexpr uint8_t kPortReset = 0x03;

/// Port table indices (mirror the periphery firmware port table).
constexpr int kEncoderBase = 0;
constexpr int kAnalogBase = 4;
constexpr int kServoBase = 10;

} // namespace communicatorCommand

/// Universal structured command for communicating with a periphery module.
///
/// The client names the physical port (e.g. "E1", "A3") and the driver
/// resolves the wire address from its own port table.  This keeps the
/// transport low-level details out of the client.
struct CommunicatorCommand {
	/// Subsystem / hardware block on the periphery module
	/// (e.g. PWM block, control block, port block).
	uint8_t device;

	/// Physical port (connector) name, e.g. "E1", "A1", "S3".
	QString portName;

	/// Operation to perform on the port (read value, set value, reset, ...).
	uint8_t operation;

	/// Command payload (e.g. power value, period and pulse, extra data).
	QByteArray data;
};

} // namespace trikControl