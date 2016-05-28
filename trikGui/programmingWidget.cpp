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

#include "programmingWidget.h"

#include <QtGui/QKeyEvent>

#include <trikKernel/paths.h>
#include <QsLog.h>

#include "commandsListWidget.h"

using namespace trikGui;

ProgrammingWidget::ProgrammingWidget(Controller &controller, QWidget *parent)
	: TrikGuiDialog(parent)
	, mTitle(tr("Add commands to list"))
	, mController(controller)
	, mEmptyCommandsCounter(1)
	, mCommandsCounter(1)
{
	mLayout.addWidget(&mTitle);

	mCommands.addItem(tr("< add command >"));
	mCommands.selectionModel()->select(
		mCommands.model()->index(0, 0)
		, QItemSelectionModel::ClearAndSelect
	);
	mCommands.addItem(tr("Run program"));
	mCommands.item(1)->setData(Qt::UserRole, QString("Run program"));
	mCommands.setFocus();
	mLayout.addWidget(&mCommands);

	setLayout(&mLayout);
}

QString ProgrammingWidget::menuEntry()
{
	return tr("Programming");
}

void ProgrammingWidget::renewFocus()
{
	mCommands.setFocus();
}

void ProgrammingWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
	case Qt::Key_PowerDown: {
		mController.abortExecution();
		break;
	}
	case Qt::Key_Return: {
		if (mCommands.currentItem()->data(Qt::UserRole) == QString("Run program")) {
			QString script = mScript;
			for (int i = 0; i < mCommands.count(); ++i) {
				script.append(mCommands.item(i)->whatsThis());
			}

			script.append(QString("    return;\n}"));
			mController.runScript(script);
			close();

		} else {
			addCommand();
		}
		break;
	}
	default: {
		TrikGuiDialog::keyPressEvent(event);
		break;
	}
	}
}

void ProgrammingWidget::addCommand()
{
	const QString text(mCommands.currentItem()->text());
	CommandsListWidget commandsListWidget(mController, text);
	emit newWidget(commandsListWidget);
	commandsListWidget.exec();

	const QString value(commandsListWidget.value());
	mCommands.currentItem()->setText(value);
	mCommands.currentItem()->setWhatsThis(commandsListWidget.script());

	if (text.startsWith("<") && value != text) {
		--mEmptyCommandsCounter;
	}

	if (mEmptyCommandsCounter == 0) {
		mCommands.item(mCommandsCounter)->setText(tr("< add command >"));
		mCommands.addItem(tr("Run program"));
		mCommands.item(mCommandsCounter)->setData(Qt::UserRole, QString());
		++mCommandsCounter;
		mCommands.item(mCommandsCounter)->setData(Qt::UserRole, QString("Run program"));
		++mEmptyCommandsCounter;
	}
}
