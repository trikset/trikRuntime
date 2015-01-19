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

#include <QtCore/QDebug>

#include <trikKernel/configurer.h>
#include <QsLog.h>

#include "configurerHelper.h"

using namespace trikControl;

ServoMotor::ServoMotor(QString const &port, trikKernel::Configurer const &configurer)
	: mDutyFile(configurer.attributeByPort(port, "deviceFile"))
	, mPeriodFile(configurer.attributeByPort(port, "periodFile"))
	, mCurrentDutyPercent(0)
	, mInvert(configurer.attributeByPort(port, "invert") == "true")
	, mCurrentPower(0)
{
	auto configure = [this, &port, &configurer](QString const &parameterName) {
		return ConfigurerHelper::configureInt(configurer, mState, port, parameterName);
	};

	mPeriod = configure("period");
	mMin = configure("min");
	mMax = configure("max");
	mZero = configure("zero");
	mStop = configure("stop");

	if (!mPeriodFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered | QIODevice::Text)) {
		QLOG_ERROR() << "Can't open motor period file " << mPeriodFile.fileName();
		mState.fail();
		return;
	}

	QString const command = QString::number(mPeriod);

	mPeriodFile.write(command.toLatin1());
	mPeriodFile.close();

	mState.ready();
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

	if (!mDutyFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered | QIODevice::Text)) {
		QLOG_ERROR() << "Can't open motor duty file " << mDutyFile.fileName();
		mState.fail();
		return;
	}

	mDutyFile.write(QString::number(mStop).toLatin1());
	mDutyFile.close();

	mCurrentPower = 0;
}

void ServoMotor::setCurrentPower(int currentPower)
{
	mCurrentPower = currentPower;
}

void ServoMotor::setCurrentDuty(int duty)
{
	mCurrentDutyPercent = 100 * duty / mPeriod;
}

void ServoMotor::writeMotorCommand(QString const &command)
{
	if (!mDutyFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered | QIODevice::Text)) {
		QLOG_ERROR() << "Can't open motor control file " << mDutyFile.fileName();
		mState.fail();
		return;
	}

	mDutyFile.write(command.toLatin1());
	mDutyFile.close();
}

int ServoMotor::min() const
{
	return mMin;
}

int ServoMotor::max() const
{
	return mMax;
}

int ServoMotor::zero() const
{
	return mZero;
}

bool ServoMotor::invert() const
{
	return mInvert;
}

bool ServoMotor::isReady() const
{
	return mState.isReady();
}
