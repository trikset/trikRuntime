/* Copyright 2014 CyberTech Labs Ltd.
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

#include "angularServoMotor.h"

#include <QtCore/QDebug>

using namespace trikControl;

AngularServoMotor::AngularServoMotor(int min, int max, int zero, int stop, QString const &dutyFile
		, QString const &periodFile, int period, bool invert)
	: ServoMotor(min, max, zero, stop, dutyFile, periodFile, period, invert)
{
}

void AngularServoMotor::setPower(int power)
{
	if (power > 90) {
		power = 90;
	} else if (power < -90) {
		power = -90;
	}

	setCurrentPower(power);

	power = invert() ? -power : power;

	int const range = power <= 0 ? zero() - min() : max() - zero();
	qreal const powerFactor = static_cast<qreal>(range) / 90;
	int duty = static_cast<int>(zero() + power * powerFactor);
	QString const command = QString::number(duty);

	setCurrentDuty(duty);

	writeMotorCommand(command);
}
