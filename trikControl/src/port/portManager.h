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
#include <QtCore/QString>

#include "portLease.h"
#include "portMode.h"

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

class PeripheryDriverInterface;
class Port;
struct PortConfig;

/// Owns all physical ports described in the system config.
///
/// The constructor parses the <ports> section and creates one Port object per
/// <port> entry: a PeripheryPort for ports served by the communicator (A*, E*)
/// and a HardwarePort for on-board ports (B*, M*). The created objects are kept
/// in a cache by port name, and the manager hands them out as PortLease objects.
///
/// Devices never access a Port directly: they acquire it for a mode through
/// this manager and release it when the lease is destroyed.
class PortManager
{
	Q_DISABLE_COPY(PortManager)

public:
	/// Creates ports from the <ports> section of @p systemConfig.
	/// @param systemConfig - path to the system config containing <ports>.
	/// @param hardwareAbstraction - HAL used by on-board (HardwarePort) ports.
	/// @param driver - periphery driver used by communicator (PeripheryPort)
	///        ports; may be null, in which case such ports are skipped with a
	///        warning.
	PortManager(const QString &systemConfig, trikHal::HardwareAbstractionInterface &hardwareAbstraction,
		PeripheryDriverInterface *driver);

	/// Destroys the owned Port objects.
	~PortManager();

	/// Acquires @p portName for @p mode on behalf of @p ownerName.
	///
	/// Re-acquiring a port for the same mode adds another reference and returns
	/// a valid lease. Returns an empty lease when the port is unknown, does not
	/// support @p mode, is already acquired for another mode, or the hardware
	/// operation fails.
	///
	/// @param portName - physical port name, for example "B7".
	/// @param mode - function the port is requested for.
	/// @param ownerName - device class or other identifier used for diagnostics.
	/// @return RAII lease that releases the port when destroyed.
	PortLease acquire(const QString &portName, PortMode mode, const QString &ownerName);

	/// Returns true when @p portName is known to the manager, that is it is
	/// described in <ports> and its Port object was created successfully.
	bool isConfigured(const QString &portName) const;

	/// Acquires the port assigned to I2C bus @p bus in PortMode::I2c.
	///
	/// The bus-to-port mapping comes from <mode name="i2c" bus="N"/> entries.
	/// Returns an empty lease when no port is assigned to @p bus or the port
	/// cannot be acquired.
	///
	/// @param bus - I2C bus number, for example 3.
	/// @param ownerName - device class or other identifier used for diagnostics.
	PortLease acquireI2c(int bus, const QString &ownerName);

private:
	/// Returns the Port object for @p name, or null when it is not configured.
	Port *port(const QString &name) const;

	/// Reads <ports> from @p systemConfig and fills the port cache and the
	/// I2C bus-to-port mapping. Invalid sections are logged and ignored.
	void parsePorts(const QString &systemConfig);

	/// Creates the implementation for @p config: PeripheryPort for communicator
	/// ports, HardwarePort otherwise. Returns null when a communicator port is
	/// requested but no periphery driver is available.
	Port *createPort(const PortConfig &config);

	/// Configured ports by name; owns the Port objects.
	QHash<QString, Port *> mPorts;

	/// Reverse mapping from I2C bus number to port name, used by acquireI2c().
	QHash<int, QString> mI2cBusToPort;

	/// HAL passed to HardwarePort objects.
	trikHal::HardwareAbstractionInterface &mHardwareAbstraction;

	/// Periphery driver passed to PeripheryPort objects; may be null.
	PeripheryDriverInterface *mDriver = nullptr;
};

}
