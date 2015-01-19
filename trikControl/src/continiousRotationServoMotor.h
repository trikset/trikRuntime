/* Copyright 2014 - 2015 CyberTech Labs Ltd.
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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QString>

#include "servoMotor.h"

namespace trikKernel {
class Configurer;
}

namespace trikControl {

/// Continious rotation servomotor.
/// @todo Merge AngularServoMotor and ContiniousRotationServoMotor to one class.
class ContiniousRotationServoMotor : public ServoMotor
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - port on which this motor is configured.
	/// @param configurer - configurer object containing preparsed XML files with motor parameters.
	ContiniousRotationServoMotor(QString const &port, trikKernel::Configurer const &configurer);

public slots:
	/// Sets current motor power to specified value, 0 to stop motor.
	/// @param power - power of the motor, from -100 (full reverse) to 100 (full forward), 0 --- break.
	void setPower(int power) override;
};

}
