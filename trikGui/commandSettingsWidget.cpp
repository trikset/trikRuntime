/* Copyright 2016 Mikhail Kita
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

#include "commandSettingsWidget.h"

#include <QtGui/QKeyEvent>

#include <trikKernel/paths.h>
#include <QsLog.h>

using namespace trikGui;

CommandSettingsWidget::CommandSettingsWidget(const QString &title, const int digits, QWidget *parent)
	: TrikGuiDialog(parent)
	, mTitle(title)
	, mValueSelector(0, digits, digits, 35, this)
{
	mContinueButton.setText(tr("Continue"));
	mValueSelector.setFocus();

	mLayout.addWidget(&mTitle);
	mLayout.addWidget(&mValueSelector);
	mLayout.addWidget(&mContinueButton);

	setLayout(&mLayout);

	mContinueButton.setAutoFillBackground(true);
	connect(&mContinueButton, SIGNAL(upPressed()), this, SLOT(focus()));
	connect(&mContinueButton, SIGNAL(downPressed()), this, SLOT(focus()));

	connect(&mValueSelector, SIGNAL(upPressed()), this, SLOT(focus()));
	connect(&mValueSelector, SIGNAL(downPressed()), this, SLOT(focus()));
}

QString CommandSettingsWidget::menuEntry()
{
	return QString(tr("Command Settings"));
}

void CommandSettingsWidget::renewFocus()
{
}

void CommandSettingsWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_Up: {
		focus();
		break;
	}
	case Qt::Key_Down: {
		focus();
		break;
	}
	case Qt::Key_Return: {
		mContinueButton.animateClick();
		exit();
		break;
	}
	default: {
		TrikGuiDialog::keyPressEvent(event);
		break;
	}
	}
}

int CommandSettingsWidget::value() const
{
	return mValueSelector.value();
}

void CommandSettingsWidget::focus()
{
	mValueSelector.hasFocusInside()
		? mContinueButton.setFocus() : mValueSelector.setFocus();
}
