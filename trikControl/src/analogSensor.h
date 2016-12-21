/* Copyright 2013 - 2015 Nikita Batov, Kseniya Gonta and CyberTech Labs Ltd.
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

class MspCommunicatorInterface;

/// Analog TRIK sensor.
class AnalogSensor : public SensorInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - port on which this sensor is configured.
	/// @param configurer - configurer object containing preparsed XML files with sensor parameters.
	/// @param communicator - I2C communicator used to query sensor.
	AnalogSensor(const QString &port, const trikKernel::Configurer &configurer, MspCommunicatorInterface &communicator);

	Status status() const override;

	int minValue() const override;

	int maxValue() const override;

public slots:
	/// Returns current reading of a sensor.
	int read() override;

	/// Returns current raw reading of a sensor.
	int readRawData() override;

private:
	enum class Type
	{
		/// Calibrated IR sensor.
		sharpGP2

		/// Normalized IR sensor.
		, analog
	};

	void calculateLNS(const QString &port, const trikKernel::Configurer &configurer);
	void calculateKB(const QString &port, const trikKernel::Configurer &configurer);

	MspCommunicatorInterface &mCommunicator;
	int mI2cCommandNumber = 0;
	Type mIRType;

	/// Linear approximation coefficient k. Normalized value is calculated as normalizedValue = k * rawValue + b.
	qreal mK = 0;

	/// Linear approximation coefficient b. Normalized value is calculated as normalizedValue = k * rawValue + b.
	qreal mB = 0;

	/// Hyperbolical approximation coefficient l.
	/// Normalized value is calculated as normalizedValue = s / (rawValue + l) + n.
	int mL = 0;

	/// Hyperbolical approximation coefficient n.
	/// Normalized value is calculated as normalizedValue = s / (rawValue + l) + n.
	int mN = 0;

	/// Hyperbolical approximation coefficient s.
	/// Normalized value is calculated as normalizedValue = s / (rawValue + l) + n.
	int mS = 0;

	/// Minimal possible normalized value returned by sensor.
	int mMinValue = 0;

	/// Maximal possible normalized value returned by sensor.
	int mMaxValue = 0;

	/// State of a device.
	DeviceState mState;
};

}
