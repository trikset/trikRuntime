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

#include "servoMotor.h"

#include <QtCore/QDebug>

using namespace trikControl;

ServoMotor::ServoMotor(int powerMin, int powerMax, QString const& controlFile, bool invert)
{
	mControlFile.setFileName(controlFile);
	mPowerMax = powerMax;
	mPowerMin = powerMin;
	mPower = 0;
}

void ServoMotor::setPower(int power)
{
	qDebug() << "Executing setPower command with power = " << power;

	if (power == 0) {
		// Quick hack for motors not stopping properly on middle power value
		powerOff();
		return;
	}

	if (!mControlFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered | QIODevice::Text)) {
		qDebug() << "Can't open motor control file " << mControlFile.fileName();
		return;
	}

	if (power > 100) {
		power = 100;
	} else if (power < -100) {
		power = -100;
	}

	mPower = power;

	QString command;

	qreal const powerFactor = static_cast<qreal>(mPowerMax - mPowerMin) / 100;
	command.sprintf("%d\n", static_cast<int>(power * powerFactor + mPowerMin));

	qDebug() << "executing: " << command;

	mControlFile.write(command.toLatin1());
	mControlFile.close();
}

int ServoMotor::power() const
{
	return mPower;
}


void ServoMotor::powerOff()
{
	qDebug() << "Executing powerOff command";

	if (!mControlFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered | QIODevice::Text)) {
		qDebug() << "Can't open motor control file " << mControlFile.fileName();
		return;
	}

	mControlFile.write("0");
	mControlFile.close();

	mPower = 0;
}
