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

#include "hardwareAbstractionInterface.h"

#include <QtCore/QScopedPointer>

namespace trikHal {
namespace trik {

/// Hardware abstraction layer for a real robot.
class TrikHardwareAbstraction : public HardwareAbstractionInterface
{
public:
	TrikHardwareAbstraction();
	~TrikHardwareAbstraction() override;

	MspI2cInterface &mspI2c() override;
	MspUsbInterface &mspUsb() override;
	SystemConsoleInterface &systemConsole() override;

	EventFileInterface *createEventFile(const QString &fileName) const override;
	FifoInterface *createFifo(const QString &fileName) const override;
	InputDeviceFileInterface *createInputDeviceFile(const QString &fileName) const override;
	OutputDeviceFileInterface *createOutputDeviceFile(const QString &fileName) const override;

private:
	/// I2C bus communicator.
	QScopedPointer<MspI2cInterface> mI2c;

	/// USB bus communicator.
	QScopedPointer<MspUsbInterface> mUsb;

	/// System console abstraction.
	QScopedPointer<SystemConsoleInterface> mSystemConsole;
};

}
}
