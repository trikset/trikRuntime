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

namespace trikControl {

/// Transport used to access a device on a given port.
enum class TransportType {
	/// Device is served directly by the onboard hardware abstraction.
	Internal,

	/// Device is served by the periphery module through the communicator.
	Communicator
};

/// Infers the transport from the physical port name.
///
/// The periphery module currently owns ports A1-A10 and E1-E4; everything else
/// is served by the onboard hardware abstraction.
inline TransportType parseTransport(const QString &port)
{
	if (port.length() >= 2 && port[0].isLetter() && port[1].isDigit()) {
		bool ok = false;
		const auto number = port.mid(1).toInt(&ok);
		if (ok) {
			const auto prefix = port[0];
			if (prefix == QLatin1Char('A') && number >= 1 && number <= 10) {
				return TransportType::Communicator;
			}

			if (prefix == QLatin1Char('E') && number >= 1 && number <= 4) {
				return TransportType::Communicator;
			}
		}
	}

	return TransportType::Internal;
}

} // namespace trikControl
