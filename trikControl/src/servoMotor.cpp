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

ServoMotor::ServoMotor(int min, int max, int zero, int stop, QString const &dutyFile, QString const &periodFile
		, int period, bool invert)
	: mDutyFile(dutyFile)
	, mPeriodFile(periodFile)
	, mPeriod(period)
	, mFrequency(1000000000 / period)
	, mCurrentDutyPercent(0)
	, mMin(min)
	, mMax(max)
	, mZero(zero)
	, mStop(stop)
	, mInvert(invert)
	, mCurrentPower(0)
{
	if (!mPeriodFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered | QIODevice::Text)) {
		qDebug() << "Can't open motor period file " << mPeriodFile.fileName();
		return;
	}

	QString const command = QString::number(mPeriod);

	mPeriodFile.write(command.toLatin1());
	mPeriodFile.close();
}

void ServoMotor::setPower(int power)
{
	if (!mDutyFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered | QIODevice::Text)) {
		qDebug() << "Can't open motor control file " << mDutyFile.fileName();
		return;
	}

	if (power > 100) {
		power = 100;
	} else if (power < -100) {
		power = -100;
	}

	mCurrentPower = power;

	power = mInvert ? -power : power;

	int const range = power <= 0 ? mZero - mMin : mMax - mZero;
	qreal const powerFactor = static_cast<qreal>(range) / 100;
	int duty = static_cast<int>(mZero + power * powerFactor);
	QString const command = QString::number(duty);

	mCurrentDutyPercent = 100 * duty / mPeriod;

	mDutyFile.write(command.toLatin1());
	mDutyFile.close();
}

int ServoMotor::power() const
{
	return mCurrentPower;
}

int ServoMotor::frequency() const
{
	return mFrequency;
}

int ServoMotor::duty() const
{
	return mCurrentDutyPercent;
}

void ServoMotor::powerOff()
{
	if (!mDutyFile.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Unbuffered | QIODevice::Text)) {
		qDebug() << "Can't open motor duty file " << mDutyFile.fileName();
		return;
	}

	mDutyFile.write(QString::number(mStop).toLatin1());
	mDutyFile.close();

	mCurrentPower = 0;
}
