/* Copyright 2015 CyberTech Labs Ltd.
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

#include <QtCore/QReadWriteLock>
#include <QtCore/QString>

#include "deviceInterface.h"

namespace trikControl {

/// Helper class to track device state (off, starting, ready, stopping, fail).
/// Thread-safe.
class DeviceState
{
public:
	/// Constructor.
	/// @param deviceName - name of the device, used for debug output.
	DeviceState(const QString &deviceName);

	/// Returns current device state.
	DeviceInterface::Status status() const;

	/// Returns true if device is in "ready" state.
	bool isReady() const;

	/// Returns true if device is in "failure" state.
	bool isFailed() const;

	/// Set "failure" state. Possible from every state. Failure is permanent. If device is in this state, other
	/// state-changing commands are ignored quietly, so device can continue initialization sequence if possible, but
	/// still be in "failure" state as a result.
	void fail();

	/// Set "starting" state. Possible only from "off" state.
	void start();

	/// Set "ready" state. Possible only from "off" and "starting" states.
	void ready();

	/// Set "stopping" state. Possible only from "ready" state.
	void stop();

	/// Set "off" state. Possible only from "ready" and "stopping" states.
	void off();

	/// Clears "failed" state and returns state to "off"
	void resetFailure();

	/// Returns name of the device for debug purposes.
	QString deviceName() const;

private:
	/// Current state of a device.
	DeviceInterface::Status mStatus = DeviceInterface::Status::off;

	/// Lock to provide thread-safe modification of device state.
	QReadWriteLock mLock;

	/// Name of the device, used for debug output.
	QString mDeviceName;
};

}
