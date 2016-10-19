/* Copyright 2014 - 2015 CyberTech Labs Ltd.
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

#include "trikGuiDialog.h"

#include <QtGui/QKeyEvent>

using namespace trikGui;

TrikGuiDialog::TrikGuiDialog(QWidget *parent)
	: MainWidget(parent)
{
}

int TrikGuiDialog::exec()
{
	show();
	return mEventLoop.exec();
}

void TrikGuiDialog::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_PowerOff: {
			goHome();
			break;
		}
		case Qt::Key_Escape: {
			exit();
			break;
		}
		default: {
			MainWidget::keyPressEvent(event);
			break;
		}
	}
}

void TrikGuiDialog::exit()
{
	close();
	mEventLoop.exit(normalExit);
}

void TrikGuiDialog::goHome()
{
	close();
	mEventLoop.exit(goHomeExit);
}
