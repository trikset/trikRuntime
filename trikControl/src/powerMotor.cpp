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
#include <math.h>
#include <QtCore/QDebug>
#include <QStringList>

#include <trikKernel/configurer.h>

#include "i2cCommunicator.h"
#include "configurerHelper.h"


	
	
int searchSuitable(int duty,int *array, int step, int length)
{
	double diff = 100.0*100.0;
	int index = length;
	int n = 0;

	for (n = 0; n < length; n++)
	{
		int newDiff = abs(duty - array[n]);

		if (newDiff < diff)
		{
			diff = newDiff;
			index = n;
		}

	}

	if (duty == 100) return (duty);
	int side = duty < array[index] ? 0 : 1;
	diff = (array[index + side] - array[index + side -1]);
	
	if (abs(diff) < 0.01)
	{
		return (index + side -1)*step;
		
	}
	else 
	{
		return((duty - array[index + side -1])*step/diff + (index + side -1)*step);
	}
}



using namespace trikControl;

PowerMotor::PowerMotor(const QString &port, const trikKernel::Configurer &configurer, I2cCommunicator &communicator)
	: mCommunicator(communicator)
	, mInvert(configurer.attributeByPort(port, "invert") == "false")
	, mCurrentPower(0)
	, listm(configurer.attributeByPort(port, "mas").split(","))
{
	mI2cCommandNumber = ConfigurerHelper::configureInt(configurer, mState, port, "i2cCommandNumber");
	mState.ready();
	mMotorType = configurer.attributeByPort(port, "type") == "motor1" ? Type::motor1 : Type::motor;
	
	int i = 0;
	int step = 1;
	lngth = listm.count();
	if (mMotorType == Type::motor1)
	{
		calc(listm);
		for ( i = 0; i <= 100; i++)
		{
			recalcDuties[i] = searchSuitable(i, calcDuties,step,lngth);

			
		}
	} else {
		for ( i = 0; i <= 100; i++)
		{
			recalcDuties[i] = i;
		}
	       }
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

void PowerMotor::calc(QStringList  me)
{	
	int k = 0;
	for (k = 0; k < me.count(); k++)
	{
		calcDuties[k] = me[k].toInt();		
	}
	for (k = 0; k < me.count(); k++)
	{
		calcDuties[k] = (int)((1.0*calcDuties[k]/calcDuties[(me.count())-1])*100);
	}
}


void PowerMotor::setPower(int power)
{
	if (power > 100) {
		power = 100;
	} else if (power < -100) {
		power = -100;
	}

	mCurrentPower = recalcDuties[power];

	power = mInvert ? -recalcDuties[power] : recalcDuties[power];

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
