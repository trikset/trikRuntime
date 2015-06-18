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

#include "powerMotor.h"

#include <QtCore/QString>

#include <trikKernel/configurer.h>
#include "i2cCommunicator.h"
#include "configurerHelper.h"
#include "exceptions/incorrectDeviceConfigurationException.h"


using namespace trikControl;

PowerMotor::PowerMotor(const QString &port, const trikKernel::Configurer &configurer, I2cCommunicator &communicator)
	: mCommunicator(communicator)
	, mInvert(configurer.attributeByPort(port, "invert") == "false")
	, mCurrentPower(0)
	, mCurrentPeriod(0x1000)
{
	mI2cCommandNumber = ConfigurerHelper::configureInt(configurer, mState, port, "i2cCommandNumber");
	calculateDutyCorrection(configurer.attributeByPort(port, "inputDuties").split(";"),mOutputDuties);
	mState.ready();
}

	
PowerMotor::~PowerMotor()
{
	if (mState.isReady()) {
		powerOff();
	}
}

PowerMotor::Status PowerMotor::status() const
{
	return combine(mCommunicator, mState.status());
}

void PowerMotor::setPower(int power)
{
	if (power > 100) {
		power = 100;
	} else if (power < -100) {
		power = -100;
	}

	if (power >= 0) {
		mCurrentPower = mOutputDuties[power];
	} else {
		mCurrentPower = -mOutputDuties[-power];	
	}
	
	power = mCurrentPower;
	
	power = mInvert ? -power: power;

	QByteArray command(2, '\0');
	command[0] = static_cast<char>(mI2cCommandNumber & 0xFF);
	command[1] = static_cast<char>(power & 0xFF);

	mCommunicator.send(command);
}

int PowerMotor::power() const
{
	return mCurrentPower;
}

int PowerMotor::period() const
{
	return mCurrentPeriod;
}

void PowerMotor::powerOff()
{
	setPower(0);
}


int PowerMotor::searchSuitable(int duty, int *array, int step, int length)
{
	auto diff = INT_MAX;
	auto index = length;

	if (duty >= 100 * mFixedPointOrder) {
		return duty;
	}
	
	for (int n = 0; n < length; n++) {
		int newDiff = abs(duty - array[n]);

		if (newDiff < diff) {
			diff = newDiff;
			index = n;
		}
	}

	const int side = duty < array[index] ? 0 : 1;
	const auto next = index + side;
	const auto prev = index + side - 1;
	diff = (array[next] - array[prev]);
	
	if (!diff) {
		return prev * step;
	} else {
        	return (duty - array[prev]) * step / diff + prev * step;
    	}     
	
}

void PowerMotor::calculateDutyCorrection(const QStringList & input,int * outputDuties)
{
	
	const int length = input.count();
	int k = 0;
	
	if (length < 2 ) {
		mState.fail();
		throw IncorrectDeviceConfigurationException("Array of input Duties shall have more than 1 values. Change string <jga25Motor /> in system-config.xml");
	} 

	
	else {
		int calcDuties [length];
		int  max = 0;
		const int step = 100 * mFixedPointOrder / (length - 1);
		
		for (k = 0; k < length; k++) {
			calcDuties[k] = input[k].toInt();
			if  (calcDuties[k] > max) {
				max = calcDuties[k];
			}
		}
		
		if (calcDuties[length - 1] != max) {
			mState.fail();
			throw IncorrectDeviceConfigurationException("Last value in inputDuties array should be maximum value. Recount encoder's values!!!");
		} else {
			for (k = 0; k < length; k++) {
				calcDuties[k] = calcDuties[k]* 100 * mFixedPointOrder / max;
			}

			for (k = 0; k <= 100; k++) {
				outputDuties[k] = searchSuitable(k * mFixedPointOrder, calcDuties,step,length) / mFixedPointOrder;
			}
		}
	}
}	

void PowerMotor::setPeriod(int period)
{
	mCurrentPeriod = period;
	QByteArray command(3, '\0');
	command[0] = static_cast<char>((mI2cCommandNumber - 4) & 0xFF);
	command[1] = static_cast<char>(period && 0xFF);
	command[2] = static_cast<char>(period >> 8);
	mCommunicator.send(command);
}
