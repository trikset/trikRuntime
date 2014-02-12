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
#include "trikGuiApplication.h"

using namespace trikGui;

MotorsWidget::MotorsWidget(QString const &configPath, QWidget *parent)
	: QWidget(parent)
	, mBrick(*TrikGuiApplication::instance()->thread(), configPath)
	, mPorts(mBrick.powerMotorPorts())
	, mLevers(mPorts.size())
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowState(Qt::WindowFullScreen);

	mPorts.sort();

	int i = 0;
	foreach (QString const &port, mPorts) {
		MotorLever *lever = new MotorLever("JM" + port, *mBrick.powerMotor(port), this);
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

QString MotorsWidget::menuEntry()
{
	return tr("Test power motors");
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
			break;
		}
	}
}
