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

#include "internal.h"

#include <trikHal/hardwareAbstractionInterface.h>
#include <trikHal/outputDeviceFileInterface.h>
#include <trikKernel/configurer.h>
#include <trikKernel/exceptions/malformedConfigException.h>

using namespace trikControl;

InternalServoMotor::InternalServoMotor(const QString &port, const trikKernel::Configurer &configurer,
	trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: ServoMotorBase(port, configurer)
{
	mDutyFile.reset(hardwareAbstraction.createOutputDeviceFile(
		configurer.attributeByPort(port, QStringLiteral("deviceFile"))));
	mPeriodFile.reset(hardwareAbstraction.createOutputDeviceFile(
		configurer.attributeByPort(port, QStringLiteral("periodFile"))));
	mRunFile.reset(hardwareAbstraction.createOutputDeviceFile(
		configurer.attributeByPort(port, QStringLiteral("runFile"))));

	if (!mDutyFile || !mPeriodFile || !mRunFile) {
		throw trikKernel::MalformedConfigException(
			"Internal servo on " + port + " requires deviceFile, periodFile, runFile");
	}

	QString capturePath;
	capturePath = configurer.attributeByPort(port, QStringLiteral("captureFile"), &capturePath);
	if (!capturePath.isEmpty()) {
		QScopedPointer<trikHal::OutputDeviceFileInterface> captureFile(
			hardwareAbstraction.createOutputDeviceFile(capturePath));
		if (!captureFile || !captureFile->open()) {
			mState.fail();
			return;
		}

		captureFile->write(QStringLiteral("0"));
	}

	if (!mPeriodFile->open()) {
		mState.fail();
		return;
	}

	if (!mRunFile->open()) {
		mState.fail();
		return;
	}

	mRunFile->write(QString::number(mRun ? 1 : 0));

	if (!mDutyFile->open()) {
		mState.fail();
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
