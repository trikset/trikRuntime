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
	mRenewTimer.setSingleShot(true);
	connect(&mRenewTimer, &QTimer::timeout, this, &BatteryIndicator::renew);

	mBeepingTimer.setSingleShot(true);
	connect(&mBeepingTimer, &QTimer::timeout, this, [this](){
		mBeepTimer.stop();
		mRenewTimer.start(mRenewInterval);
	});

	connect(&mBeepTimer, &QTimer::timeout, this, [this](){
		mBrick.playTone(1500, 100);
	});

	renew();
}

void BatteryIndicator::renew()
{
	if (mBrick.battery()->status() == trikControl::DeviceInterface::Status::ready) {
		const auto voltage = mBrick.battery()->readVoltage();
		if (voltage > mSanityThreshold && voltage < shutdownThreshold()) {
			QProcess::startDetached("/bin/sh", {"-c", "halt"});
		} else if (voltage > mSanityThreshold && voltage < warningThreshold()) {
			mBeepTimer.start(500);
			mBeepingTimer.start(mBeepingInterval);
		}
		setText(QString::number(voltage, 'f', 1) + " V");
		mRenewTimer.start(mRenewInterval);
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
