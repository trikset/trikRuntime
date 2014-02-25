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

#include <QtCore/QDebug>
#include <QtGui/QKeyEvent>

#include "motorLever.h"
#include "trikGuiApplication.h"

using namespace trikGui;

MotorsWidget::MotorsWidget(QString const &configPath
		, trikControl::Motor::Type type
		, QWidget *parent
		)
	: QWidget(parent)
	, mBrick(*TrikGuiApplication::instance()->thread(), configPath)
	, mPorts(mBrick.motorPorts(type))
	, mLevers(mPorts.size())
{
	setWindowState(Qt::WindowFullScreen);

	mPorts.sort();

	int i = 0;
	foreach (QString const &port, mPorts) {
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

QString MotorsWidget::menuEntry(trikControl::Motor::Type type)
{
	switch (type) {
		case trikControl::Motor::powerMotor: {
			return tr("Test power motors");
			break;
		}
		case trikControl::Motor::servoMotor: {
			return tr("Test servo motors");
			break;
		}
	}

	return QString();
}

void MotorsWidget::exec()
{
	show();
	mEventLoop.exec();
}

void MotorsWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Left: {
			focusPreviousChild();
			break;
		}
		case Qt::Key_Right: {
			focusNextChild();
			break;
		}
		case Qt::Key_Meta: case Qt::Key_PowerDown: {
			close();
			mEventLoop.quit();
			break;
		}
		default: {
			QWidget::keyPressEvent(event);
			break;
		}
	}
}
