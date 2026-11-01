/* Copyright 2013 - 2015 Matvey Bryksin, Yurii Litvinov and CyberTech Labs Ltd.
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

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>

#include "batteryInterface.h"
#include "deviceState.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
class InputDeviceFileInterface;
}

namespace trikControl {

/// Implements battery voltage sensor for real hardware.
/// Reads the battery channel through an IIO device (SARADC + analog mux).
class /* TRIKCONTROL_EXPORT for ScriptRunner/PythonQt */ TRIKCONTROL_EXPORT Battery : public BatteryInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param configurer - configurer object with the battery device file.
	/// @param hardwareAbstraction - hardware abstraction used to open the IIO channel.
	Battery(const trikKernel::Configurer &configurer,
		const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	~Battery() override;

	Status status() const override;

public Q_SLOTS:
	float readVoltage() override;

	float readRawDataVoltage() override;

private:
	QScopedPointer<trikHal::InputDeviceFileInterface> mVoltageFile;
	DeviceState mState;
	double mScaleMv = 1.7578125;
	double mDivider = 1.0;
};

}
