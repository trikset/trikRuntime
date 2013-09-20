/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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

	showCurrentDir();
}

FileManagerWidget::~FileManagerWidget()
{
}

QString FileManagerWidget::menuEntry()
{
	return "File Manager";
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
		case Qt::Key_Meta: {
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
