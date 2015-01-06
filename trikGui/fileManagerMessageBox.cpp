/* Copyright 2014 Cybertech Labs Ltd.
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

#include "fileManagerMessageBox.h"

#include <QtGui/QKeyEvent>

using namespace trikGui;

FileManagerMessageBox::FileManagerMessageBox(QWidget *parent)
	: QMessageBox(parent)
{
	init();
}

FileManagerMessageBox::~FileManagerMessageBox()
{
	delete mDeleteButton;
	delete mOpenButton;
}

void FileManagerMessageBox::init()
{
	setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
	setIcon(QMessageBox::Question);
	setText(tr("Do you want to open or delete the file?"));

	mOpenButton = addButton(tr("Open"), QMessageBox::AcceptRole);
	mDeleteButton = addButton(tr("Delete"), QMessageBox::DestructiveRole);
}

void FileManagerMessageBox::showMessage()
{
	mEscStatus = false;
	setDefaultButton(mOpenButton);
	exec();
}

FileManagerMessageBox::FileState FileManagerMessageBox::userAnswer() const
{
	if (!mEscStatus) {
		QAbstractButton const* const button = clickedButton();
		if (button == mOpenButton) {
			return FileState::Open;
		} else if (button == mDeleteButton) {
			return FileState::Delete;
		}
	}

	return FileState::None;
}

void FileManagerMessageBox::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Escape: {
			mEscStatus = true;
			reject();
			break;
		}
		case Qt::Key_Left:
		case Qt::Key_Right: {
			changeDefaultButton();
			break;
		}
		default: {
			QMessageBox::keyPressEvent(event);
			break;
		}
	}
}

void FileManagerMessageBox::changeDefaultButton()
{
	QPushButton const* const current = defaultButton();
	if (current == mOpenButton) {
		setDefaultButton(mDeleteButton);
	} else {
		setDefaultButton(mOpenButton);
	}
}
