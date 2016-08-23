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

namespace trikControl {

/// TRIK device interface, base interface for everything that can be plugged to a brick or already on board.
class DeviceInterface
{

public:
	virtual ~DeviceInterface() {}

	/// Device status.
	enum class Status {
		/// Device can not be turned on due to hardware or configuration failure that will likely not be fixed at least
		/// until reboot.
		permanentFailure

		/// Device is not ready due to a hardware or software failure that can fix itself at some time in the future,
		/// so the device will try to reinitialize itself from time to time.
		, temporaryFailure

		/// Device is turned off.
		, off

		/// Device received command to turn on, but is starting now, and it may take some time.
		, starting

		/// Device is ready and working properly.
		, ready

		/// Device received command to turn off and stopping now, it may take some time.
		, stopping
	};

	/// Helper method to return status of a device relying on other device to work. If first device is ready, status of
	/// a second device is returned, otherwise it is status of a first device.
	static Status combine(const DeviceInterface &underlying, const DeviceInterface::Status &dependent)
	{
		const DeviceInterface::Status status = underlying.status();
		if (status != DeviceInterface::Status::ready) {
			return status;
		}

		return dependent;
	}

	/// Returns current status of the device.
	virtual Status status() const = 0;
};

}
