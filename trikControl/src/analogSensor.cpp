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

#include "analogSensor.h"

#include <trikKernel/configurer.h>
#include <QsLog.h>

#include "mspCommunicatorInterface.h"
#include "configurerHelper.h"

using namespace trikControl;

AnalogSensor::AnalogSensor(const QString &port, const trikKernel::Configurer &configurer
		, MspCommunicatorInterface &communicator)
	: mCommunicator(communicator)
	, mState("Analog Sensor on" + port)
{
	mI2cCommandNumber = ConfigurerHelper::configureInt(configurer, mState, port, "i2cCommandNumber");
	mIRType = configurer.attributeByPort(port, "type") == "SharpGP2" ? Type::sharpGP2 : Type::analog;
	mMinValue = ConfigurerHelper::configureInt(configurer, mState, port, "minValue");
	mMaxValue = ConfigurerHelper::configureInt(configurer, mState, port, "maxValue");

	if (configurer.attributeByPort(port, "filter") == "median3") {
		mDataFilter.reset(new DataFilter(mMinValue, mMaxValue, "median3"));
	}

	// We use linear subjection to normalize common analog sensor values:
	// normalizedValue = k * rawValue + b
	// To calculate k and b we need two raw values and two corresponding them normalized values.

	// We use hyperbolical subjection to normalize SharpGP2 sensor values:
	// normalizedValue = s / (rawValue + l) + n
	// To calculate s, l and n we need sensor readings at three distances.

	if (mIRType == Type::sharpGP2){
		calculateLNS(port, configurer);
	} else {
		calculateKB(port, configurer);
	}

	mState.ready();
}

AnalogSensor::Status AnalogSensor::status() const
{
	return DeviceInterface::combine(mCommunicator, mState.status());
}

int AnalogSensor::read()
{
	const auto raw = readRawData();
	auto result = raw;
	if (mIRType == Type::sharpGP2) {
		const auto quotient = raw + mL;
		result = quotient != 0 ? mS / quotient + mN : 0;
	}
	else {
		result = mK * raw + mB;
	}

	return !mDataFilter ? result : mDataFilter->applyFilter(result);
}

int AnalogSensor::readRawData()
{
	if (!mState.isReady() || mCommunicator.status() != DeviceInterface::Status::ready) {
		return 0;
	}

	QByteArray command(2, '\0');
	command[0] = static_cast<char>(mI2cCommandNumber & 0xFF);
	command[1] = static_cast<char>((mI2cCommandNumber >> 8) & 0xFF);

	return mCommunicator.read(command);
}

void AnalogSensor::calculateLNS(const QString &port, const trikKernel::Configurer &configurer)
{
	QStringList result;
	for (const QString &str : configurer.attributeByPort(port, "values").split(")")) {
		result += str.mid(1).split(";");
	}

	const int x1 = result[0].toInt();
	const int y1 = result[1].toInt();
	const int x2 = result[2].toInt();
	const int y2 = result[3].toInt();
	const int x3 = result[4].toInt();
	const int y3 = result[5].toInt();

	// Counted from equations x1 = mS/(y1 + mL) + mN, x2 = mS/(y2 + mL) + mN, x3 = mS/(y3 + mL) + mN
	mL = (-x1 * y1 * y3 - x3 * y2 * y3 + x3 * y1 * y3 + x1 * y1 * y2 + x2 * y2 * y3 - x2 * y1 * y2) /
			(x1 * y3 - x2 * y3 + x2 * y1 - x1 * y2 + x3 * y2 - x3 * y1);

	mS = (x1 - x2) * (y1 + mL) * (y2 + mL) / (y2 - y1);
	mN = x1 - mS / (y1 + mL);
}

void AnalogSensor::calculateKB(const QString &port, const trikKernel::Configurer &configurer)
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

int AnalogSensor::minValue() const
{
	return mMinValue;
}

int AnalogSensor::maxValue() const
{
	return mMaxValue;
}
