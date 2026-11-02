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

#include <functional>

#include <QtCore/QHash>
#include <QtCore/QString>

namespace trikControl {

class DeviceInterface;

/// Small runtime factory mapping a configured device class name
/// (for example, "servoMotor") to a creation function.
///
/// The creation function receives the port name and is responsible for
/// selecting the transport (onboard or periphery) when the device supports
/// both. Unknown classes are simply ignored.
class DeviceFactory
{
public:
	using Creator = std::function<DeviceInterface *(const QString &port)>;

	/// Registers a creator for a device class. A later registration for the
	/// same class replaces the previous one.
	void registerDevice(const QString &deviceClass, Creator creator);

	/// Creates a device of the given class on the given port.
	/// Returns nullptr when the class is unknown or the creator decides that
	/// the device cannot be created.
	DeviceInterface *create(const QString &deviceClass, const QString &port) const;

private:
	QHash<QString, Creator> mCreators;
};

} // namespace trikControl
