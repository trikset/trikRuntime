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

#include "motorsWidget.h"

#include <QtGui/QKeyEvent>

#include "motorLever.h"

using namespace trikGui;

MotorsWidget::MotorsWidget(trikControl::BrickInterface &brick
		, trikControl::MotorInterface::Type type
		, QWidget *parent)
	: TrikGuiDialog(parent)
	, mBrick(brick)
	, mPorts(mBrick.motorPorts(type))
	, mLevers(mPorts.size())
{
	mPorts.sort();

	int i = 0;
	for (const QString &port : mPorts) {
		MotorLever *lever = new MotorLever(port, *mBrick.motor(port), this);
		mLayout.addWidget(lever);
		mLevers[i] = lever;
		++i;
	}

	setLayout(&mLayout);
}

MotorsWidget::~MotorsWidget()
{
	qDeleteAll(mLevers);
}

QString MotorsWidget::menuEntry(trikControl::MotorInterface::Type type)
{
	switch (type) {
	case trikControl::MotorInterface::Type::powerMotor: {
		return tr("Test power motors");
	}
	case trikControl::MotorInterface::Type::servoMotor: {
		return tr("Test servo motors");
	}
	}

	return QString();
}

void MotorsWidget::renewFocus()
{
	if (!mLevers.isEmpty()) {
		mLevers.first()->setFocus();
	}
}

void MotorsWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Up: {
			focusPreviousChild();
			break;
		}
		case Qt::Key_Down: {
			focusNextChild();
			break;
		}
		default: {
			TrikGuiDialog::keyPressEvent(event);
			break;
		}
	}
}
