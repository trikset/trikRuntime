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

#include "batteryInterface.h"

namespace trikControl {

class I2cCommunicator;

/// Implements battery voltage sensor for real hardware.
class Battery : public BatteryInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param communicator - I2C communicator to use to query battery status.
	Battery(I2cCommunicator &communicator);

	Status status() const override;

public slots:
	float readVoltage() override;

	float readRawDataVoltage() override;

private:
	I2cCommunicator &mCommunicator;
};

}
