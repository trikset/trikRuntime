/* Copyright 2014 Roman Kurbatov
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

#include "motorLever.h"

#include <QtGui/QKeyEvent>

#include <QtWidgets/QStylePainter>
#include <QtWidgets/QStyleOptionFocusRect>


#include <trikControl/motorInterface.h>
#include <abstractIndicator.h>

using namespace trikGui;

MotorLever::MotorLever(const QString &port, trikControl::MotorInterface &motor, QWidget *parent)
	: QFrame(parent)
	, mMotor(motor)
	, mIsOn(false)
	, mMaxPower(motor.maxControl())
	, mMinPower(motor.minControl())
	, mPowerStep(10)
	, mPower(0)
	, mNameLabel(port)
	, mOnOffLabel(tr("off"))
{
	mMotor.powerOff();

	mPowerBar.setOrientation(Qt::Horizontal);
	mPowerBar.setMinimum(mMinPower);
	mPowerBar.setMaximum(mMaxPower);
	mPowerBar.setValue(0);
	mPowerBar.setTextVisible(true);
	mPowerBar.setFormat("%v");

	mNameLabel.setAlignment(Qt::AlignLeft);
	mPowerBar.setAlignment(Qt::AlignCenter);
	mOnOffLabel.setAlignment(Qt::AlignRight);

	// mOnOffLabel can change its width during work. It will cause mPowerBar
	// width change. To prevent it, we set fixed width it.
	mOnOffLabel.setFixedWidth(std::max(AbstractIndicator::fontMetricsHorizontalAdvance(this, tr("off"))
									   , AbstractIndicator::fontMetricsHorizontalAdvance(this, tr("on"))));

	mLayout.addWidget(&mNameLabel);
	mLayout.addWidget(&mPowerBar);
	mLayout.addWidget(&mOnOffLabel);
	setLayout(&mLayout);

	setFocusPolicy(Qt::StrongFocus);
	setSizePolicy(QSizePolicy::Expanding, QSizePolicy::QSizePolicy::MinimumExpanding);
}

MotorLever::~MotorLever()
{
	mMotor.powerOff();
}

void MotorLever::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Right: {
			setPower(mPower + mPowerStep);
			break;
		}
		case Qt::Key_Left: {
			setPower(mPower - mPowerStep);
			break;
		}
		case Qt::Key_Return: {
			turnOnOff();
			break;
		}
		default: {
			QWidget::keyPressEvent(event);
		}
	}
}

void MotorLever::setPower(int power)
{
	if (power > mMaxPower || power < mMinPower) {
		return;
	}

	mPower = power;
	mPowerBar.setValue(power);
	if (mIsOn) {
		mMotor.setPower(power);
	}
}

void MotorLever::turnOnOff()
{
	mIsOn = !mIsOn;
	if (mIsOn) {
		mOnOffLabel.setText(tr("on"));
		mMotor.setPower(mPower);
	} else {
		mOnOffLabel.setText(tr("off"));
		mMotor.powerOff();
	}
}
