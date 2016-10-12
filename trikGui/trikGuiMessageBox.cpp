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

#include "trikGuiMessageBox.h"

#include <QtGui/QKeyEvent>

using namespace trikGui;

TrikGuiMessageBox::TrikGuiMessageBox(QWidget *parent)
	: MainWidget(parent)
{
	mLayout.addWidget(&mMessageLabel);
	setLayout(&mLayout);

	mMessageLabel.setAlignment(Qt::AlignCenter);
}


int TrikGuiMessageBox::exec(const QString &message)
{
	mMessageLabel.setText(message);
	show();
	return mEventLoop.exec();
}

void TrikGuiMessageBox::renewFocus()
{
	setFocus();
}

void TrikGuiMessageBox::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_PowerOff: {
			close();
			mEventLoop.exit(1);
			break;
		}
		case Qt::Key_Return:
		case Qt::Key_Escape: {
			close();
			mEventLoop.quit();
			break;
		}
		default:
			MainWidget::keyPressEvent(event);
	}
}
