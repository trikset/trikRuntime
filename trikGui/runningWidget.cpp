/* Copyright 2013 Yurii Litvinov
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

#include "runningWidget.h"

#include <QtGui/QKeyEvent>

using namespace trikGui;

RunningWidget::RunningWidget(QString const &programName, Controller &controller, QWidget *parent)
	: QWidget(parent)
	, mController(controller)
{
	setWindowState(Qt::WindowFullScreen);

	mProgramNameLabel.setText(tr("Running:") + "\n" + programName);
	mAbortLabel.setText(tr("Press Enter\n to abort"));

	mLayout.addWidget(&mProgramNameLabel);
	mLayout.addWidget(&mAbortLabel);

	setLayout(&mLayout);
}

void RunningWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Enter: {
			mController.abortExecution();
			close();
			break;
		}
		default: {
			QWidget::keyPressEvent(event);
			break;
		}
	}
}
