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

#include "analogSensor.h"


#include "i2cCommunicator.h"

const int analogMax = 1024;  //for all analog sensors
const int analogMin = 0;  //for all analog sensors

using namespace trikControl;

AnalogSensor::AnalogSensor(I2cCommunicator &communicator, int i2cCommandNumber)
	: mCommunicator(communicator)
	, mI2cCommandNumber(i2cCommandNumber)
{
}

int AnalogSensor::read()
{
	QByteArray command(2, '\0');
	command[0] = static_cast<char>(mI2cCommandNumber & 0xFF);

	int value = mCommunicator.read(command);

	value = qMin(value, analogMax);
	value = qMax(value, analogMin);

	double const scale = 100.0 / (static_cast<double>(analogMax - analogMin));

	value = (value - analogMin) * scale;

	return value;
}
