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

#include <QtCore/QHashFunctions>

namespace trikControl {

/// Function that a physical port can perform.
///
/// The same connector may support several mutually exclusive functions,
/// selected at runtime. A mode describes the hardware function, not the device
/// class: for example, both a servo motor and a PWM capture device use Pwm.
enum class PortMode {
	/// Port is free.
	None,

	/// PWM output and capture.
	Pwm,

	/// UART TX/RX.
	Uart,

	/// General-purpose input/output.
	Gpio,

	/// I2C bus.
	I2c,

	/// Analog-to-digital converter input.
	Adc
};

inline uint qHash(PortMode key, uint seed = 0) noexcept
{
	return ::qHash(static_cast<int>(key), seed);
}

}
