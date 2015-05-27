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

AnalogSensor::AnalogSensor(const QString &port, const trikKernel::Configurer &configurer, I2cCommunicator &communicator)
	: mCommunicator(communicator)
{
	mI2cCommandNumber = ConfigurerHelper::configureInt(configurer, mState, port, "i2cCommandNumber");
	mIRType = configurer.attributeByPort(port, "type") == "SharpGP2" ? Type::SharpGP2 : Type::Analog;
	// We use linear subjection to normalize sensor values:
	// normalizedValue = k * rawValue + b
	// To calculate k and b we need two raw values and two corresponding them normalized values.

	

	if (mIRType == Type::SharpGP2){
		CalculateLNS(port, configurer);	
	} else {
			CalculateKB(port, configurer);	
	       }


	mState.ready();
}

AnalogSensor::Status AnalogSensor::status() const
{
	return DeviceInterface::combine(mCommunicator, mState.status());
}


int AnalogSensor::read()
{
	auto raw = readRawData();
	if (mIRType == Type::sharpGP2){
		return mS/(raw + mL) + mN;
		
	}
	return mK * raw + mB;
		

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



void AnalogSensor::CalculateLNS(const QString &port, const trikKernel::Configurer &configurer)
{
	const long long x1 = ConfigurerHelper::configureInt(configurer, mState, port, "x1");
	const long long x2 = ConfigurerHelper::configureInt(configurer, mState, port, "x2");
	const long long x3 = ConfigurerHelper::configureInt(configurer, mState, port, "x3");
	const auto y1 = ConfigurerHelper::configureInt(configurer, mState, port, "y1");
	const auto y2 = ConfigurerHelper::configureInt(configurer, mState, port, "y2");
	const auto y3 = ConfigurerHelper::configureInt(configurer, mState, port, "y3");
	const auto d1 = x3*(y1 - y3)*(x2 - x1);
	const auto d2 = x1*x2;
	const auto d3 = x2*x3;
	const auto d4 = x3 - x1 - (y1 - y3)*(x2 - x1);
	mN = -(d1 + (d2 - d3)*(y1 - y2)) / d4;
	const auto d5 = (y1 - y2)*(x1 - mN)*(x2 - mN);
	const auto d6 = (x3 - mN) * (x2 - x1);
	mL = d5 / d6 - y3;
	mS = (y1 - y2)*(x2 - mN)*(x1 - mN) / (x2 - x1);
}



void AnalogSensor::CalculateKB(const QString &port, const trikKernel::Configurer &configurer)
{
	const int rawValue1 = ConfigurerHelper::configureInt(configurer, mState, port, "rawValue1");
	const int rawValue2 = ConfigurerHelper::configureInt(configurer, mState, port, "rawValue2");
	const int normalizedValue1 = ConfigurerHelper::configureInt(configurer, mState, port, "normalizedValue1");
	const int normalizedValue2 = ConfigurerHelper::configureInt(configurer, mState, port, "normalizedValue2");
	if (rawValue1 == rawValue2) {
		QLOG_ERROR() <<  "Sensor calibration error: rawValue1 = rawValue2!";
		mState.fail();
		mK = 0;
		mB = 0;
	} else {
		mK = static_cast<qreal>(normalizedValue2 - normalizedValue1) / (rawValue2 - rawValue1);
		mB = normalizedValue1 - mK * rawValue1;
		}
}








