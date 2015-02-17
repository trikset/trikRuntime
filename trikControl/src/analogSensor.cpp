/* Copyright 2013 - 2015 Nikita Batov and CyberTech Labs Ltd.
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

#include <trikKernel/configurer.h>
#include <QsLog.h>

#include "i2cCommunicator.h"
#include "configurerHelper.h"

using namespace trikControl;

AnalogSensor::AnalogSensor(QString const &port, trikKernel::Configurer const &configurer, I2cCommunicator &communicator)
	: mCommunicator(communicator)
{
	mI2cCommandNumber = ConfigurerHelper::configureInt(configurer, mState, port, "i2cCommandNumber");

	// We use linear subjection to normalize sensor values:
	// normalizedValue = k * rawValue + b
	// To calculate k and b we need two raw values and two corresponding them normalized values.

	int const rawValue1 = ConfigurerHelper::configureInt(configurer, mState, port, "rawValue1");
	int const rawValue2 = ConfigurerHelper::configureInt(configurer, mState, port, "rawValue2");
	int const normalizedValue1 = ConfigurerHelper::configureInt(configurer, mState, port, "normalizedValue1");
	int const normalizedValue2 = ConfigurerHelper::configureInt(configurer, mState, port, "normalizedValue2");

	if (rawValue1 == rawValue2) {
		QLOG_ERROR() <<  "Sensor calibration error: rawValue1 = rawValue2!";
		mState.fail();
		mK = 0;
		mB = 0;
	} else {
		mK = static_cast<qreal>(normalizedValue2 - normalizedValue1) / (rawValue2 - rawValue1);
		mB = normalizedValue1 - mK * rawValue1;
	}

	mState.ready();
}

AnalogSensor::Status AnalogSensor::status() const
{
	return DeviceInterface::combine(mCommunicator, mState.status());
}

int AnalogSensor::read()
{
	if (!mState.isReady() || mCommunicator.status() != DeviceInterface::Status::ready) {
		return 0;
	}

    QByteArray command(1, '\0');
	command[0] = static_cast<char>(mI2cCommandNumber & 0xFF);

	return mK * mCommunicator.read(command) + mB;
}

int AnalogSensor::readRawData()
{
	if (!mState.isReady() || mCommunicator.status() != DeviceInterface::Status::ready) {
		return 0;
	}

    QByteArray command(1, '\0');
	command[0] = static_cast<char>(mI2cCommandNumber & 0xFF);

	return mCommunicator.read(command);
}
