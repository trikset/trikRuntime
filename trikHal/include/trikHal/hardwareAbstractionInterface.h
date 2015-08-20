/* Copyright 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include "inputDeviceFileInterface.h"
#include "outputDeviceFileInterface.h"
#include "eventFileInterface.h"
#include "fifoInterface.h"
#include "i2cInterface.h"
#include "systemConsoleInterface.h"

namespace trikHal {

/// Hardware abstraction, provides devices that are used to communicate with robot hardware or emulate it.
class HardwareAbstractionInterface
{
public:
	virtual ~HardwareAbstractionInterface() {}

	/// Returns I2C bus implementation.
	virtual I2CInterface &i2c() = 0;

	/// Returns system console wrapper, able to execute system command and launch processes.
	virtual SystemConsoleInterface &systemConsole() = 0;

	/// Creates new event file, passes ownership to a caller.
	virtual EventFileInterface *createEventFile() const = 0;

	/// Creates new FIFO, passes ownership to a caller.
	virtual FifoInterface *createFifo() const = 0;

	/// Creates new input event file, passes ownership to a caller.
	virtual InputDeviceFileInterface *createInputDeviceFile(const QString &fileName) const = 0;

	/// Creates new output event file, passes ownership to a caller.
	virtual OutputDeviceFileInterface *createOutputDeviceFile(const QString &fileName) const = 0;
};

}
