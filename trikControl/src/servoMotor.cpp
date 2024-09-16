/* Copyright 2013 - 2014 Yurii Litvinov, CyberTech Labs Ltd.
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

#include <trikKernel/configurer.h>
#include <trikHal/hardwareAbstractionInterface.h>
#include <QsLog.h>

#include "configurerHelper.h"

using namespace trikControl;

ServoMotor::ServoMotor(const QString &port, const trikKernel::Configurer &configurer
	, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mDutyFile(hardwareAbstraction.createOutputDeviceFile(configurer.attributeByPort(port, "deviceFile")))
	, mPeriodFile(hardwareAbstraction.createOutputDeviceFile(configurer.attributeByPort(port, "periodFile")))
	, mRunFile(hardwareAbstraction.createOutputDeviceFile(configurer.attributeByPort(port, "runFile")))
	, mCurrentDutyPercent(0)
	, mInvert(configurer.attributeByPort(port, "invert") == "true")
	, mCurrentPower(0)
	, mRun(false)
	, mState("Servomotor on " + port)
{
	const auto configure = [this, &port, &configurer](const QString &parameterName) {
		return ConfigurerHelper::configureInt(configurer, mState, port, parameterName);
	};

	mPeriod = configure("period");
	mMin = configure("min");
	mMax = configure("max");
	mZero = configure("zero");
	mStop = configure("stop");
	mMinControlRange = configure("controlMin");
	mMaxControlRange = configure("controlMax");

	if (!mPeriodFile->open()) {
		mState.fail();
		return;
	}

	if (!mRunFile->open()) {
		mState.fail();
		return;
	} else {
		mRunFile->write(QString::number(mRun ? 1 : 0));
	}

	setPeriod(mPeriod / 1000);

	if (!mDutyFile->open()) {
		mState.fail();
	} else {
		mState.ready();
	}
}

ServoMotor::~ServoMotor()
{
}

ServoMotor::Status ServoMotor::status() const
{
	return mState.status();
}

int ServoMotor::minControl() const
{
	return mMinControlRange;
}

int ServoMotor::maxControl() const
{
	return mMaxControlRange;
}

int ServoMotor::power() const
{
	return mCurrentPower;
}

int ServoMotor::frequency() const
{
	return 1000000000.0 / static_cast<qreal>(mPeriod);
}

int ServoMotor::duty() const
{
	return mCurrentDutyPercent;
}

void ServoMotor::powerOff()
{
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to power off motor which is not ready, ignoring";
		return;
	}

	mDutyFile->write(QString::number(mStop));
	mRunFile->write(QString::number(0));
	mRun = false;
	mCurrentPower = 0;

	mRun = false;
	mRunFile->write(QString::number(mRun));
}

void ServoMotor::setPeriod(int uSec)
{
	const QString command = QString::number(uSec * 1000);
	mPeriodFile->write(command);
}

void ServoMotor::setPower(int power, bool constrain)
{
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to turn on motor which is not ready, ignoring";
		return;
	}

	if (constrain) {
		if (power > mMaxControlRange) {
			power = mMaxControlRange;
		} else if (power < mMinControlRange) {
			power = mMinControlRange;
		}
	}

	mCurrentPower = power;

	const int meanControlRange = (mMaxControlRange + mMinControlRange) / 2;

	power = (mInvert ? -1 : 1) * (power - meanControlRange) + meanControlRange;

	const int range = power <= meanControlRange ? mZero - mMin : mMax - mZero;

	const qreal powerFactor = static_cast<qreal>(range) / (mMaxControlRange - mMinControlRange) * 2;
	const int duty = static_cast<int>(mZero + (power - meanControlRange) * powerFactor);
	const QString command = QString::number(duty);

	mCurrentDutyPercent = 100 * duty / mPeriod;

	mDutyFile->write(command);

	if (!mRun) {
		mRun = true;
		mRunFile->write(QString::number(mRun));
	}
}
