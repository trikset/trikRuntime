/* Copyright 2014 Roman Kurbatov
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

#include "motorLever.h"

#include <trikControl/motorInterface.h>
#include <abstractIndicator.h>

using namespace trikGui;

MotorLever::MotorLever(const QString &port, trikControl::MotorInterface &motor, QObject *parent)
	: QObject(parent), mMotor(motor), mIsOn(false), mMaxPower(motor.maxControl()), mMinPower(motor.minControl()),
	  mPowerStep(10), mPower(0), mNameLabel(port) {
	mMotor.powerOff();
}

MotorLever::~MotorLever() { mMotor.powerOff(); }

QString MotorLever::nameLabel() { return mNameLabel; }

int MotorLever::maxPower() { return mMaxPower; }
int MotorLever::minPower() { return mMinPower; }
int MotorLever::powerStep() { return mPowerStep; }
int MotorLever::power() { return mPower; }

void MotorLever::setPower(int power) {

	if (power > mMaxPower || power < mMinPower) {
		return;
	}

	mPower = power;

	if (mIsOn) {
		mMotor.setPower(power);
	}
	Q_EMIT powerChanged();
}

void MotorLever::setIsOn(bool isOn) {
	mIsOn = isOn;
	if (mIsOn) {
		mMotor.setPower(mPower);
	} else {
		mMotor.powerOff();
	}
	Q_EMIT isOnChanged();
}

bool MotorLever::isOn() { return mIsOn; }
