/* Copyright 2013 Roman Kurbatov
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
 * limitations under the License.
 *
 * This file was modified by Yurii Litvinov to make it comply with the requirements of trikRuntime
 * project. See git revision history for detailed changes. */

#include "fileManagerWidget.h"

#include <QtGui/QKeyEvent>

using namespace trikGui;

FileManagerWidget::FileManagerWidget(Controller &controller, QWidget *parent)
	: QWidget(parent)
	, mController(controller)
{
	setAttribute(Qt::WA_DeleteOnClose);
	setWindowState(Qt::WindowFullScreen);

	mFileSystemModel.setRootPath("/");
	mFileSystemModel.setFilter(QDir::AllEntries | QDir::System | QDir::NoDot);

	mFileSystemView.setModel(&mFileSystemModel);

	mLayout.addWidget(&mCurrentPathLabel);
	mLayout.addWidget(&mFileSystemView);
	setLayout(&mLayout);

	QDir::setCurrent("./scripts");

	showCurrentDir();
}

FileManagerWidget::~FileManagerWidget()
{
}

QString FileManagerWidget::menuEntry()
{
	return tr("File Manager");
}

void FileManagerWidget::open()
{
	 QModelIndex const &index = mFileSystemView.currentIndex();
	 if (mFileSystemModel.isDir(index)) {
		 QDir::setCurrent(mFileSystemModel.filePath(index));
		 showCurrentDir();
	 } else {
		 mController.runFile(mFileSystemModel.filePath(index));
	 }
}

void FileManagerWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Left: {
			close();
			break;
		}
		case Qt::Key_Enter: {
			open();
			break;
		}
		default: {
			QWidget::keyPressEvent(event);
			break;
		}
	}
}

void FileManagerWidget::showCurrentDir()
{
	mCurrentPathLabel.setText(QDir::currentPath());
	mFileSystemView.setRootIndex(mFileSystemModel.index(QDir::currentPath()));
}
