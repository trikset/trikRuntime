/* Copyright 2025 CyberTech Labs Ltd.
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

enum class TransportType {
	Internal
	, Communicator
};

namespace detail {

/// Returns true if the port consists of the given letter prefix followed by a
/// number within [min; max], e.g. "A5" with ('A', 1, 6) -> true.
inline bool isPortInRange(const QString &port, QChar prefix, int min, int max)
{
	if (port.length() < 2 || port[0] != prefix) {
		return false;
	}

	bool ok = false;
	const int number = port.mid(1).toInt(&ok);
	return ok && number >= min && number <= max;
}

} // namespace detail

/// Infer transport policy from the port (connector) name.
///
/// Ports physically wired to the periphery module talk over the communicator;
/// everything else is served by the on-module hardware directly.
inline TransportType parseTransport(const QString &port)
{
	// MSP periphery module ports.
	if (detail::isPortInRange(port, 'A', 1, 6)
		|| detail::isPortInRange(port, 'E', 1, 4)
		|| detail::isPortInRange(port, 'M', 1, 4)) {
		return TransportType::Communicator;
	}

	return TransportType::Internal;
}

} // namespace trikControl