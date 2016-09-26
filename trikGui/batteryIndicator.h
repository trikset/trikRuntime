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

#pragma once

#include <QtCore/qglobal.h>
#include <QtCore/QTimer>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QLabel>
#else
	#include <QtWidgets/QLabel>
#endif

#include <trikControl/brickInterface.h>
#include "powerLevel.h"

namespace trikGui {

/// Label showing battery voltage.
class BatteryIndicator : public QLabel
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param brick - object that provides interface to a hardware.
	/// @param parent - parent of this widget in terms of Qt parent-child widget relations.
	explicit BatteryIndicator(trikControl::BrickInterface &brick, QWidget *parent = 0);

private slots:
	void renew();

private:
	/// Provides current low voltage warning threshold (in volts) based on currently selected power level.
	float warningThreshold() const;

	/// Provides current low voltage shutdown threshold (in volts) based on currently selected power level.
	float shutdownThreshold() const;

	/// Reference to an underlying Brick object which provides access to a battery voltage.
	trikControl::BrickInterface &mBrick;

	/// Timer that renews battery status once in a while.
	QTimer mRenewTimer;

	/// Battery status renewal interval.
	const int mRenewInterval = 60 * 1000;

	/// Currently selected low power threshold level on a board.
	PowerLevel::Level mCurrentLevel;

	/// Warning voltage threshold for 12V mode (robot begins to beep like mad if battery voltage is lower than
	/// this value).
	const float m12VWarningThreshold = 10.9;

	/// Shutdown voltage threshold for 12V mode (robot turns off correctly if battery voltage is lower than
	/// this value).
	const float m12VShutdownThreshold = 10.4;

	/// Warning voltage threshold for 6V mode (robot begins to beep like mad if battery voltage is lower than
	/// this value).
	const float m6VWarningThreshold = 7.1;

	/// Shutdown voltage threshold for 6V mode (robot turns off correctly if battery voltage is lower than
	/// this value).
	const float m6VShutdownThreshold = 6.8;

	/// Voltage threshold that defines lowest possible voltage reading that is considered correct. So if for some
	/// reason battery voltage is not available, indicator will not panic and turn off robot.
	const float mSanityThreshold = 1.0;
};

}
