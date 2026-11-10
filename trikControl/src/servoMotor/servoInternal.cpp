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

#include "servoInternal.h"

#include <QsLog.h>

#include <trikHal/hardwareAbstractionInterface.h>
#include <trikHal/outputDeviceFileInterface.h>
#include <trikKernel/configurer.h>

using namespace trikControl;

InternalServoMotor::InternalServoMotor(const QString &port, const trikKernel::Configurer &configurer,
	trikHal::HardwareAbstractionInterface &hardwareAbstraction, PortManager &ports)
	: ServoMotorBase(port, configurer, ports)
{
	if (mState.isFailed()) {
		return;
	}

	const auto pwmDevice = mPortLease.device();
	if (pwmDevice.isEmpty()) {
		QLOG_ERROR() << "Internal servo on" << port << "requires a PWM device on the port";
		failSetup();
		return;
	}

	mPwmChannel = PwmChannelLease(pwmDevice, hardwareAbstraction);
	if (!mPwmChannel) {
		QLOG_ERROR() << "Internal servo on" << port << "cannot resolve PWM device" << pwmDevice;
		failSetup();
		return;
	}

	const auto channelDir = mPwmChannel.channelDir();
	mDutyFile.reset(hardwareAbstraction.createOutputDeviceFile(channelDir + QStringLiteral("/duty_cycle")));
	mPeriodFile.reset(hardwareAbstraction.createOutputDeviceFile(channelDir + QStringLiteral("/period")));
	mRunFile.reset(hardwareAbstraction.createOutputDeviceFile(channelDir + QStringLiteral("/enable")));

	if (!mDutyFile || !mPeriodFile || !mRunFile) {
		QLOG_ERROR() << "Internal servo on" << port << "cannot create the PWM channel files";
		failSetup();
		return;
	}

	if (!mPeriodFile->open()) {
		failSetup();
		return;
	}

	if (!mRunFile->open()) {
		failSetup();
		return;
	}

	mRunFile->write(QString::number(mRun ? 1 : 0));

	if (!mDutyFile->open()) {
		failSetup();
		return;
	}

	mDutyFile->write(QStringLiteral("0"));
	markReady();
	setPeriod(mPeriod / 1000);
}

InternalServoMotor::~InternalServoMotor()
{
	if (mState.isReady()) {
		powerOff();
	}
}

void InternalServoMotor::failSetup()
{
	mState.fail();
	mPortLease.release();
	mPwmChannel.release();
}

void InternalServoMotor::applyPeriod(uint32_t periodNs)
{
	mPeriodFile->write(QString::number(periodNs));
}

void InternalServoMotor::applyDuty(int pulseNs)
{
	mDutyFile->write(QString::number(pulseNs));
}

void InternalServoMotor::applyRun(bool on)
{
	mRunFile->write(QString::number(on ? 1 : 0));
}

void InternalServoMotor::applyPowerOff()
{
	mRunFile->write(QStringLiteral("0"));
	mDutyFile->write(QStringLiteral("0"));
	mPeriodFile->write(QStringLiteral("0"));
}
