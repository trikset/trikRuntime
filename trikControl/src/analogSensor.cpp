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

const int analogMax = 1024;  //for all analog sensors
const int analogMin = 0;  //for all analog sensors

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
	qDebug() << "rawValue1=" << rawValue1;
	qDebug() << "rawValue2=" << rawValue2;
	qDebug() << "normalizedValue1=" << normalizedValue1;
	qDebug() << "normalizedValue2=" << normalizedValue2;
	if (rawValue1 == rawValue2) {
		qDebug() << "Distance sensor error: rawValue1 = rawValue2!";
		k = 0;
		b = 0;
	} else {
		k = static_cast<double>(normalizedValue2 - normalizedValue1) / (rawValue2 - rawValue1);
		b = normalizedValue1 - k * rawValue1;
	}
}

int AnalogSensor::read()
{
	QByteArray command(1, '\0');
	command[0] = static_cast<char>(mI2cCommandNumber & 0xFF);

	int value = k * mCommunicator.read(command) + b;

	return value;
}
