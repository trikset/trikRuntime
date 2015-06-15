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
#include "exceptions/incorrectDeviceConfigurationException.h"

#include "deviceInterface.h"
#include "deviceState.h"

#include <QsLog.h>
#include <math.h>
#include <QtCore/QDebug>
#include <QStringList>
#include <limits.h>
#include <trikKernel/configurer.h>

#include "i2cCommunicator.h"
#include "configurerHelper.h"

	

using namespace trikControl;

PowerMotor::PowerMotor(const QString &port, const trikKernel::Configurer &configurer, I2cCommunicator &communicator)
	: mCommunicator(communicator)
	, mInvert(configurer.attributeByPort(port, "invert") == "false")
	, mCurrentPower(0)
	
{
	mI2cCommandNumber = ConfigurerHelper::configureInt(configurer, mState, port, "i2cCommandNumber");
	mState.ready();

	calculateDutyCorrection(configurer.attributeByPort(port, "inputDuties").split(";"),outputDuties);


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

	mCurrentPower = outputDuties[power];

	power = mInvert ? -outputDuties[power] : outputDuties[power];

	QByteArray command(2, '\0');
	command[0] = static_cast<char>(mI2cCommandNumber & 0xFF);
	command[1] = static_cast<char>(power & 0xFF);

	mCommunicator.send(command);
}

int PowerMotor::power() const
{
	return mCurrentPower;
}

void PowerMotor::powerOff()
{
	setPower(0);
}



int PowerMotor::searchSuitable(int duty,int *array, int step, int length)
{
	auto diff = INT_MAX;
	auto index = length;

	if (duty >= (100 * fixedPointOrder))
		return duty;
	
	for (int n = 0; n < length; n++){
		int newDiff = abs(duty - array[n]);

		if (newDiff < diff){
			diff = newDiff;
			index = n;
		}

	}

  	
	int side = duty < array[index] ? 0 : 1;
	auto next = index + side;
	auto prev = index + side - 1;
	diff = (array[next] - array[prev]);
	
	if (!diff){
		return prev * step;
	}
	else{
        	return (duty - array[prev]) * step / diff + prev * step;
    	}     
	
}

void PowerMotor::calculateDutyCorrection (QStringList const & input,int * outputDuties)
{
	
	int k = 0;
	int length = input.count();
	if (length < 2 ) {
		//mstate.fail();
		//QLOG_ERROR() << "array of input Duties shall have more than 1 value";
		throw IncorrectDeviceConfigurationException("array of input Duties shall have more than 1 values");
	} 
	else{
		int calcDuties [length];
		int  max = 0;
		int step = 100 * fixedPointOrder / (length - 1);
		for (k = 0; k < length; k++)
		{
			calcDuties[k] = input[k].toInt();
			if  (calcDuties[k] > max)
			{
				max = calcDuties[k];
			}
		}
	
		for (k = 0; k < length; k++)
		{
			calcDuties[k] = calcDuties[k]* 100 * fixedPointOrder / max;
		}
	

		for ( k = 0; k <= 100; k++)
		{
			outputDuties[k] = searchSuitable(k * fixedPointOrder, calcDuties,step,length) / fixedPointOrder;
		}
	}
}




