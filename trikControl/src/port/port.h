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

#include <QtCore/QHash>
#include <QtCore/QList>
#include <QtCore/QString>

#include "portMode.h"

namespace trikControl {

/// Hardware resources of one port mode.
struct PortModeConfig {
	/// Mode-specific device reference: PWM controller name for Pwm,
	/// serial device for Uart and so on.
	QString device;

	/// I2C bus number for the I2c mode, -1 when not applicable.
	int bus = -1;
};

/// Static description of a physical port.
struct PortConfig {
	/// Physical port name, for example "B1".
	QString name;

	/// Per-mode hardware resources.
	QHash<PortMode, PortModeConfig> modes;
};

/// A physical multi-function connector.
///
/// Owns the resources shared by all functions of the connector: pinmux
/// selection, port power and exclusivity. It does not configure anything
/// function-specific (PWM duty/period, tty, capture mode); that is the
/// responsibility of the device that acquired the port.
class Port
{
public:
	Port() = default;

	virtual ~Port() = default;

	/// Returns the physical port name, for example "B1".
	virtual QString name() const = 0;

	/// Returns modes this port can perform.
	virtual QList<PortMode> allowedModes() const = 0;

	/// Returns true if the port supports @p mode.
	virtual bool allows(PortMode mode) const = 0;

	/// Returns the mode the port is currently acquired for, or PortMode::None.
	virtual PortMode activeMode() const = 0;

	/// Returns true if the port is currently acquired by someone.
	virtual bool isActive() const = 0;

	/// Returns the identifier of the current owner, empty when the port is free.
	virtual QString owner() const = 0;

	/// Returns the resource of the current mode, or an empty string.
	virtual QString activeDevice() const = 0;

	/// Acquires the port for @p mode: enables power, applies the mode pinmux and
	/// records @p owner. Returns a non-zero owner token, or 0 when the port is
	/// busy, does not support @p mode, or the hardware operation failed.
	/// @param owner - device class or other identifier used for diagnostics.
	virtual quint64 start(PortMode mode, const QString &owner) = 0;

	/// Releases the port: applies the idle pinmux, disables power and clears the
	/// owner. Does nothing when @p token is not the current owner token.
	/// Does not touch function-specific device state (PWM duty/period, open
	/// tty); the device must quiesce before calling this.
	virtual void stop(quint64 token) = 0;
};

}
