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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

#include "sensorInterface.h"
#include "deviceState.h"

namespace trikKernel {
class Configurer;
}

namespace trikControl {

class I2cCommunicator;

/// Analog TRIK sensor.
class AnalogSensor : public SensorInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - port on which this sensor is configured.
	/// @param configurer - configurer object containing preparsed XML files with sensor parameters.
	/// @param communicator - I2C communicator used to query sensor.
	AnalogSensor(QString const &port, trikKernel::Configurer const &configurer, I2cCommunicator &communicator);

	Status status() const override;

public slots:
	/// Returns current reading of a sensor.
	int read();

	/// Returns current raw reading of a sensor.
	int readRawData() override;

private:
	I2cCommunicator &mCommunicator;
	int const mI2cCommandNumber;

	/// Linear approximation coefficient k. Normalized value is calculated as normalizedValue = k * rawValue + b.
	qreal mK;

	/// Linear approximation coefficient b. Normalized value is calculated as normalizedValue = k * rawValue + b.
	qreal mB;

	/// State of a device.
	DeviceState mState;
};

}
