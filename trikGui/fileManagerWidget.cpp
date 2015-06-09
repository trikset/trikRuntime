/* Copyright 2013 - 2015 Roman Kurbatov and CyberTech Labs Ltd.
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

#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtGui/QKeyEvent>
#include <QtCore/QSettings>

using namespace trikGui;

FileManagerWidget::FileManagerWidget(Controller &controller, MainWidget::FileManagerRootType fileManagerRoot
		, QWidget *parent)
	: TrikGuiDialog(parent)
	, mController(controller)
{
	if (fileManagerRoot == MainWidget::FileManagerRootType::allFS) {
		mRootDirPath = QDir::rootPath();
	} else { // if (fileManagerRoot == MainWidget::FileManagerRootType::scriptsDir)
		mRootDirPath = mController.scriptsDirPath();
	}

	QDir::setCurrent(mController.startDirPath());
	QDir dir;
	dir.mkdir(mController.scriptsDirName());
	QDir::setCurrent(mController.scriptsDirPath());

	mFileSystemModel.setRootPath(mRootDirPath);
	mFileSystemModel.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDot);

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

	connect(mFileSystemView.selectionModel(), SIGNAL(currentChanged(QModelIndex, QModelIndex))
			, this, SLOT(onSelectionChanged(QModelIndex, QModelIndex)));

	QSettings settings("trik");
	mLastSelectedFile = settings.value("lastSelectedFile").toString();

	showCurrentDir();
}

FileManagerWidget::~FileManagerWidget()
{
	QSettings settings("trik");
	settings.setValue("lastSelectedFile", mLastSelectedFile);
}

QString FileManagerWidget::menuEntry()
{
	return tr("File Manager");
}

void FileManagerWidget::renewFocus()
{
	mFileSystemView.setFocus();
}

void FileManagerWidget::open()
{
	const QModelIndex &index = mFileSystemView.currentIndex();
	if (mFileSystemModel.isDir(index)) {
		if (QDir::setCurrent(mFileSystemModel.filePath(index))) {
			showCurrentDir();
		}
	} else {
		mOpenDeleteBox.showMessage();
		FileManagerMessageBox::FileState const choice = mOpenDeleteBox.userAnswer();
		switch (choice) {
		case FileManagerMessageBox::FileState::Open:
			mController.runFile(mFileSystemModel.filePath(index));
			break;
		case FileManagerMessageBox::FileState::Delete:
			mFileSystemModel.remove(index);
			break;
		default:
			break;
		}
	}
}

void FileManagerWidget::keyPressEvent(QKeyEvent *event)
{
	switch (event->key()) {
		case Qt::Key_Return: {
			open();
			break;
		}
		default: {
			TrikGuiDialog::keyPressEvent(event);
			break;
		}
	}
}

void FileManagerWidget::onSelectionChanged(QModelIndex current, QModelIndex previous)
{
	Q_UNUSED(previous);

	mLastSelectedFile = mFileSystemModel.filePath(current);
}

QString FileManagerWidget::currentPath()
{
	QString result = QDir::currentPath();
	if (mRootDirPath != "/") {
		result.replace(0, mRootDirPath.length(), "");
	}

	if (result.isEmpty()) {
		result = "/";
	}

	return result;
}

void FileManagerWidget::showCurrentDir()
{
	mCurrentPathLabel.setText(currentPath());

	QDir::Filters filters = mFileSystemModel.filter();
	if (QDir::currentPath() == mRootDirPath) {
		filters |= QDir::NoDotDot;
	} else {
		filters &= ~QDir::NoDotDot;
	}

	filters &= ~QDir::Hidden;

	mFileSystemModel.setFilter(filters);

	mFileSystemView.setRootIndex(mFileSystemModel.index(QDir::currentPath()));
}

void FileManagerWidget::onDirectoryLoaded(const QString &path)
{
	if (QDir::currentPath() != path) {
		return;
	}

	mFileSystemModel.sort(0);

	renewCurrentIndex();
}

void FileManagerWidget::renewCurrentIndex()
{
	mFileSystemView.setFocus();

	QModelIndex currentIndex;
	if (!mLastSelectedFile.isEmpty()) {
		currentIndex = mFileSystemModel.index(mLastSelectedFile);
		if (currentIndex.parent() != mFileSystemView.rootIndex()) {
			// If last selected file is not in this directory, ignore it.
			currentIndex = QModelIndex();
		}
	}

	if (!currentIndex.isValid()) {
		currentIndex = mFileSystemModel.index(0, 0, mFileSystemView.rootIndex());
	}

	mFileSystemView.selectionModel()->select(currentIndex, QItemSelectionModel::ClearAndSelect);
	mFileSystemView.setCurrentIndex(currentIndex);
	mFileSystemView.scrollTo(currentIndex, QAbstractItemView::PositionAtCenter);
}
