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

#include "batteryIndicator.h"

#include <QtCore/QString>
#include <QtCore/QProcess>

using namespace trikGui;

BatteryIndicator::BatteryIndicator(trikControl::BrickInterface &brick, QWidget *parent)
	: QLabel(parent)
	, mBrick(brick)
	, mCurrentLevel(PowerLevel::currentLevel())
{
	renew();

	mRenewTimer.setInterval(mRenewInterval);
	mRenewTimer.setSingleShot(false);
	connect(&mRenewTimer, SIGNAL(timeout()), this, SLOT(renew()));
	mRenewTimer.start();
}

void BatteryIndicator::renew()
{
	if (mBrick.battery()->status() == trikControl::DeviceInterface::Status::ready) {
		const auto voltage = mBrick.battery()->readVoltage();
		if (voltage > mSanityThreshold && voltage < shutdownThreshold()) {
			QProcess::startDetached("/bin/sh", {"-c", "halt"});
		} else if (voltage > mSanityThreshold && voltage < warningThreshold()) {
			mBrick.playTone(800, mRenewInterval / 2);
		}

		setText(QString::number(voltage, 'f', 1) + " V");
	}
}

float BatteryIndicator::warningThreshold() const
{
	return mCurrentLevel == PowerLevel::Level::twelveVolt ? m12VWarningThreshold : m6VWarningThreshold;
}

float BatteryIndicator::shutdownThreshold() const
{
	return mCurrentLevel == PowerLevel::Level::twelveVolt ? m12VShutdownThreshold : m6VShutdownThreshold;
}
