/* Copyright 2013 Yurii Litvinov
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

#include <QtCore/QDebug>

using namespace trikControl;

PowerMotor::PowerMotor(QString const &command, QString const &stop, bool invert)
	: mCommand(command)
	, mStopCommand(stop)
	, mInvert(invert)
	, mCurrentPower(0)
{
}

void PowerMotor::setPower(int power)
{
	if (power > 100) {
		power = 100;
	} else if (power < -100) {
		power = -100;
	}

	mCurrentPower = power;

	power = mInvert ? -power : power;

	char command[100] = {0};

	sprintf(command, mCommand.toStdString().c_str(), static_cast<unsigned char>(power));

	system(command);
}

int PowerMotor::power() const
{
	return mCurrentPower;
}

void PowerMotor::powerOff()
{
	system(mStopCommand.toStdString().c_str());
	mCurrentPower = 0;
}
