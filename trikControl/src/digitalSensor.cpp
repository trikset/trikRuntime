/* Copyright 2013 - 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include "digitalSensor.h"

#include <trikKernel/configurer.h>
#include <trikHal/hardwareAbstractionInterface.h>
#include <QsLog.h>

#include "configurerHelper.h"

using namespace trikControl;

DigitalSensor::DigitalSensor(const QString &port, const trikKernel::Configurer &configurer
	, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mDeviceFile(hardwareAbstraction.createInputDeviceFile(configurer.attributeByPort(port, "deviceFile")))
	, mState("Digital Sensor on" + port)
{
	mMin = ConfigurerHelper::configureInt(configurer, mState, port, "min");
	mMax = ConfigurerHelper::configureInt(configurer, mState, port, "max");

	mState.ready();

	// Testing availability of a device.
	read();
}

DigitalSensor::~DigitalSensor()
{
}

int DigitalSensor::read()
{
	if (!mState.isReady()) {
		return 0;
	}

	int value = readRawData();

	if (mState.isFailed()) {
		return 0;
	}

	if (mMax == mMin) {
		return mMin;
	}

	value = qMin(value, mMax);
	value = qMax(value, mMin);

	const qreal scale = 100.0 / (static_cast<qreal>(mMax - mMin));

	value = (value - mMin) * scale;

	return value;
}

int DigitalSensor::readRawData()
{
	if (!mState.isReady()) {
		return 0;
	}

	if (!mDeviceFile->open()) {
		mState.fail();
		return 0;
	}

	int value = 0;
	mDeviceFile->stream() >> value;

	mDeviceFile->close();

	return value;
}

DigitalSensor::Status DigitalSensor::status() const
{
	return mState.status();
}

int DigitalSensor::minValue() const
{
	return 0;
}

int DigitalSensor::maxValue() const
{
	return 100;
}
