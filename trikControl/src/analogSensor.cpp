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
	mIRType = configurer.attributeByPort(port, "type") == "IR" ? Type::IR : Type::CustomIR;
	// We use linear subjection to normalize sensor values:
	// normalizedValue = k * rawValue + b
	// To calculate k and b we need two raw values and two corresponding them normalized values.

	

	if (mIRType == Type::CustomIR){
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
	if (!mState.isReady() || mCommunicator.status() != DeviceInterface::Status::ready) {
		return 0;
	}
	QByteArray command(1, '\0');
	command[0] = static_cast<char>(mI2cCommandNumber & 0xFF);
	if (mIRType == Type::IR){
		return mK * mCommunicator.read(command) + mB;
	}
	
	return mS/((mCommunicator.read(command)) + mL) + mN;

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
	const float x1 = ConfigurerHelper::configureInt(configurer, mState, port, "x1");
	const float x2 = ConfigurerHelper::configureInt(configurer, mState, port, "x2");
	const float x3 = ConfigurerHelper::configureInt(configurer, mState, port, "x3");
	const float y1 = ConfigurerHelper::configureInt(configurer, mState, port, "y1");
	const float y2 = ConfigurerHelper::configureInt(configurer, mState, port, "y2");
	const float y3 = ConfigurerHelper::configureInt(configurer, mState, port, "y3");
	const float d1 = (x3*(y1 - y3)*(x2 - x1)) / (y1 - y2);
	const float d2 = x1*x2;
	const float d3 = x2*x3;
	const float d4 = x3 - x1 - (y1 - y3)*(x2 - x1) / (y1 - y2);
	const float d5 = (y1 - y2)*(x1 + mL)*(x2 + mL);
	const float d6 = (x3 + mL) * (x2 - x1);
	mL = (d1 + d2 - d3) / d4;
	mN = y3 - d5 / d6;
	mS = (y1 - y2)*(x2 + mL)*(x1 + mL) / (x2 - x1);
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








