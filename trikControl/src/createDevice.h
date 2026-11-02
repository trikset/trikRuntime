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

#include <QtCore/QString>

#include <QsLog.h>

#include "transports/transportType.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

class PeripheryDriverInterface;

/// Creates a device implementation selected by the port name.
///
/// Devices that can be served both by the onboard hardware abstraction and by
/// the periphery module provide two concrete implementations. The point of
/// entry stays the same: the transport is inferred from the port name.
///
/// Returns nullptr when a periphery implementation is requested but no
/// periphery driver is available.
template<typename InternalDevice, typename CommunicatorDevice, typename Interface>
Interface *makeDevice(const QString &port, const trikKernel::Configurer &configurer,
	trikHal::HardwareAbstractionInterface &hardwareAbstraction, PeripheryDriverInterface *driver)
{
	if (parseTransport(port) == TransportType::Communicator) {
		if (!driver) {
			QLOG_ERROR() << "No periphery driver for device on port" << port;
			return nullptr;
		}

		return new CommunicatorDevice(port, configurer, *driver);
	}

	return new InternalDevice(port, configurer, hardwareAbstraction);
}

} // namespace trikControl
