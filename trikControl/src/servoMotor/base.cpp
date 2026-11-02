/* Copyright 2026 CyberTech Labs Ltd.
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

#include "base.h"

#include <trikKernel/configurer.h>

#include <QsLog.h>

#include "../configurerHelper.h"

using namespace trikControl;

ServoMotorBase::ServoMotorBase(const QString &port, const trikKernel::Configurer &configurer)
	: mState("Servomotor on " + port)
	, mPort(port)
{
	const auto configure = [&](const QString &parameterName) {
		return ConfigurerHelper::configureInt(configurer, mState, port, parameterName);
	};

	mPeriod = configure(QStringLiteral("period"));
	mMin = configure(QStringLiteral("min"));
	mMax = configure(QStringLiteral("max"));
	mZero = configure(QStringLiteral("zero"));
	mStop = configure(QStringLiteral("stop"));
	mMinControlRange = configure(QStringLiteral("controlMin"));
	mMaxControlRange = configure(QStringLiteral("controlMax"));

	mInvert = configurer.attributeByPort(port, QStringLiteral("invert")) == QStringLiteral("true");
}

ServoMotorBase::~ServoMotorBase() = default;

ServoMotorBase::Status ServoMotorBase::status() const
{
	return mState.status();
}

int ServoMotorBase::minControl() const
{
	return mMinControlRange;
}

int ServoMotorBase::maxControl() const
{
	return mMaxControlRange;
}

int ServoMotorBase::frequency() const
{
	return static_cast<int>(1000000000.0 / static_cast<qreal>(mPeriod));
}

int ServoMotorBase::duty() const
{
	return mCurrentDutyPercent;
}

int ServoMotorBase::power() const
{
	return mCurrentPower;
}

// NOLINTNEXTLINE(google-default-arguments)
void ServoMotorBase::setPower(int power, bool constrain)
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
	const int pulseNs = static_cast<int>(mZero + (power - meanControlRange) * powerFactor);

	mCurrentDutyPercent = 100 * pulseNs / mPeriod;

	if (!mRun) {
		mRun = true;
		applyRun(true);
	}

	applyDuty(pulseNs);
}

void ServoMotorBase::powerOff()
{
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to power off motor which is not ready, ignoring";
		return;
	}

	applyPowerOff();
	mRun = false;
	mCurrentPower = 0;
}

// NOLINTNEXTLINE(google-default-arguments)
void ServoMotorBase::setPeriod(int uSec)
{
	applyPeriod(static_cast<uint32_t>(uSec) * 1000);
}

const QString &ServoMotorBase::port() const
{
	return mPort;
}

void ServoMotorBase::markReady()
{
	mState.ready();
}
