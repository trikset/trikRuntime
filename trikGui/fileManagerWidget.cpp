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
	setWindowState(Qt::WindowFullScreen);

	mFileSystemModel.setRootPath("/");
	mFileSystemModel.setFilter(QDir::AllEntries | QDir::System | QDir::NoDot);

	connect(&mFileSystemModel
			, SIGNAL(directoryLoaded(QString))
			, this
			, SLOT(onDirectoryLoaded(QString))
			);

	mFileSystemView.setModel(&mFileSystemModel);

	mLayout.addWidget(&mCurrentPathLabel);
	mLayout.addWidget(&mFileSystemView);
	setLayout(&mLayout);

	mFileSystemView.setSelectionMode(QAbstractItemView::SingleSelection);
	mFileSystemView.setFocus();

	mCurrentDir = QDir().exists("./scripts") ? "./scripts" : ".";

	showCurrentDir();
}

FileManagerWidget::~FileManagerWidget()
{
}

QString FileManagerWidget::menuEntry()
{
	return tr("File Manager");
}

int FileManagerWidget::exec()
{
	show();
	return mEventLoop.exec();
}

void FileManagerWidget::open()
{
	 QModelIndex const &index = mFileSystemView.currentIndex();
	 if (mFileSystemModel.isDir(index)) {
		 mCurrentDir = mFileSystemModel.filePath(index);
		 showCurrentDir();
	 } else {
		 mController.runFile(mFileSystemModel.filePath(index));
	 }
}

void FileManagerWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_PowerDown: {
			close();
			mEventLoop.exit(1);
			break;
		}
		case Qt::Key_Escape: {
			close();
			mEventLoop.quit();
			break;
		}
		case Qt::Key_Return: {
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
	mCurrentPathLabel.setText(QDir(mCurrentDir).path());
	mFileSystemView.setRootIndex(mFileSystemModel.index(QDir(mCurrentDir).path()));
	renewCurrentIndex();
}

void FileManagerWidget::onDirectoryLoaded(QString const &path)
{
	if (QDir(mCurrentDir).absolutePath() != path) {
		return;
	}

	renewCurrentIndex();
}

void FileManagerWidget::renewCurrentIndex()
{
	mFileSystemView.setFocus();

	QModelIndex const currentIndex = mFileSystemModel.index(
			0
			, 0
			, mFileSystemModel.index(QDir(mCurrentDir).absolutePath())
			);

	mFileSystemView.selectionModel()->select(currentIndex, QItemSelectionModel::ClearAndSelect);
	mFileSystemView.setCurrentIndex(currentIndex);
}
