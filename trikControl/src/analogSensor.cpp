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

#include <QtCore/QDebug>

#include "i2cCommunicator.h"

using namespace trikControl;

AnalogSensor::AnalogSensor(I2cCommunicator &communicator
		, int i2cCommandNumber
		, int rawValue1
		, int rawValue2
		, int normalizedValue1
		, int normalizedValue2)
	: mCommunicator(communicator)
	, mI2cCommandNumber(i2cCommandNumber)
{
	// We use linear subjection to normalize sensor values:
	// normalizedValue = k * rawValue + b
	// To calculate k and b we need two raw values and two corresponding them normalized values.

	if (rawValue1 == rawValue2) {
		qDebug() << "Sensor calibration error: rawValue1 = rawValue2!";
		mK = 0;
		mB = 0;
	} else {
		mK = static_cast<double>(normalizedValue2 - normalizedValue1) / (rawValue2 - rawValue1);
		mB = normalizedValue1 - mK * rawValue1;
	}
}

int AnalogSensor::read()
{
	QByteArray command(1, '\0');
	command[0] = static_cast<char>(mI2cCommandNumber & 0xFF);

	int value = mK * mCommunicator.read(command) + mB;

	return value;
}

int AnalogSensor::readRawData()
{
	QByteArray command(1, '\0');
	command[0] = static_cast<char>(mI2cCommandNumber & 0xFF);

	return mCommunicator.read(command);
}
