/* Copyright 2013 Nikita Batov
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
#include <QtCore/QString>

#include "declSpec.h"
#include "sensor.h"

namespace trikControl {

class I2cCommunicator;

/// Analog TRIK sensor.
class TRIKCONTROL_EXPORT AnalogSensor : public Sensor
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param communicator - I2C communicator to use to query sensor.
	/// @param i2cCommandNumber - number of i2c command corresponding to that sensor.
	AnalogSensor(I2cCommunicator &communicator, int i2cCommandNumber);

public slots:
	/// Returns current raw reading of a sensor.
	int read();

private:
	I2cCommunicator &mCommunicator;
	int const mI2cCommandNumber;
};

}
