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

#include <trikKernel/exceptions/trikRuntimeException.h>

#include "deviceInterface.h"

#include <QsLog.h>

namespace trikControl {

/// Exception for incorrect device state change, for example, from "off" to "stopping".
class IncorrectStateChangeException : public trikKernel::TrikRuntimeException
{
public:
	/// Constructor.
	/// @param deviceName - name of the device.
	/// @param from - status from which we tried to change.
	/// @param to - status to which we tried to change.
	IncorrectStateChangeException(const QString &deviceName, DeviceInterface::Status from, DeviceInterface::Status to)
		: trikKernel::TrikRuntimeException(QString("%1: incorrect device state change from \"%2\" to \"%3\"")
					.arg(deviceName).arg(statusToString(from)).arg(statusToString(to)))
	{
	}

	/// Constructor, used to report incorrect resetting of failure state.
	/// @param deviceName - name of the device.
	/// @param from - status from which we tried to change.
	IncorrectStateChangeException(const QString &deviceName, DeviceInterface::Status from)
		: trikKernel::TrikRuntimeException(QString("%1 : incorrect attempt to reset failure from \"%2\"")
										   .arg(deviceName).arg(statusToString(from)))
	{
	}

private:
	/// Helper method to convert status enum value to string.
	static QString statusToString(DeviceInterface::Status status) {
		switch (status) {
		case DeviceInterface::Status::permanentFailure:
			return "permanent failure";
		case DeviceInterface::Status::temporaryFailure:
			return "temporary failure";
		case DeviceInterface::Status::off:
			return "off";
		case DeviceInterface::Status::ready:
			return "ready";
		case DeviceInterface::Status::starting:
			return "starting";
		case DeviceInterface::Status::stopping:
			return "stopping";
		}

		return "";
	}
};

}
