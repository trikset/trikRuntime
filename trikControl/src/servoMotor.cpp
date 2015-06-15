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
#include <QsLog.h>

#include "configurerHelper.h"

using namespace trikControl;

ServoMotor::ServoMotor(const QString &port, const trikKernel::Configurer &configurer)
	: mDutyFile(configurer.attributeByPort(port, "deviceFile"))
	, mPeriodFile(configurer.attributeByPort(port, "periodFile"))
	, mCurrentDutyPercent(0)
	, mInvert(configurer.attributeByPort(port, "invert") == "true")
	, mCurrentPower(0)
{
	auto configure = [this, &port, &configurer](const QString &parameterName) {
		return ConfigurerHelper::configureInt(configurer, mState, port, parameterName);
	};

	mPeriod = configure("period");
	mMin = configure("min");
	mMax = configure("max");
	mZero = configure("zero");
	mStop = configure("stop");

	mMotorType = configurer.attributeByPort(port, "type") == "angular" ? Type::angular : Type::continiousRotation;

	if (!mPeriodFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered | QIODevice::Text)) {
		QLOG_ERROR() << "Can't open motor period file " << mPeriodFile.fileName();
		mState.fail();
		return;
	}

	const QString command = QString::number(mPeriod);

	mPeriodFile.write(command.toLatin1());
	mPeriodFile.close();

	if (!mDutyFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered | QIODevice::Text)) {
		QLOG_ERROR() << "Can't open motor control file " << mDutyFile.fileName();
		mState.fail();
	} else {
		mState.ready();
	}
}

ServoMotor::~ServoMotor()
{
	mDutyFile.close();
}

ServoMotor::Status ServoMotor::status() const
{
	return mState.status();
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

	mDutyFile.write(QString::number(mStop).toLatin1());
	mCurrentPower = 0;
}

void ServoMotor::setPower(int power)
{
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to turn on motor which is not ready, ignoring";
		return;
	}

	const int powerBoundary = mMotorType == Type::angular ? 90 : 100;

	if (power > powerBoundary) {
		power = powerBoundary;
	} else if (power < -powerBoundary) {
		power = -powerBoundary;
	}

	mCurrentPower = power;

	power = mInvert ? -power : power;

	const int range = power <= 0 ? mZero - mMin : mMax - mZero;
	const qreal powerFactor = static_cast<qreal>(range) / 90;
	int duty = static_cast<int>(mZero + power * powerFactor);
	const QString command = QString::number(duty);

	mCurrentDutyPercent = 100 * duty / mPeriod;

	mDutyFile.write(command.toLatin1());
}
