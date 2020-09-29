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

#include <QtCore/qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	#include <QtGui/QMessageBox>
#else
	#include <QtWidgets/QMessageBox>
#endif

#include <trikKernel/paths.h>
#include <trikKernel/exceptions/trikRuntimeException.h>
#include <QsLog.h>

using namespace trikGui;

FileManagerWidget::FileManagerWidget(Controller &controller, MainWidget::FileManagerRootType fileManagerRoot
		, QWidget *parent)
	: TrikGuiDialog(parent)
	, mFileIconProvider(new LightFileIconProvider())
	, mController(controller)
{
	QDir dir(trikKernel::Paths::userScriptsPath());

	if (!dir.exists()) {
		const bool result = dir.mkpath(trikKernel::Paths::userScriptsPath());
		if (!result) {
			QLOG_ERROR() << "Incorrect user scripts directory";
		}
	}

	if (dir.exists()) {
		QDir::setCurrent(trikKernel::Paths::userScriptsPath());
	} else {
		// Fallback directory, if user scripts are unavailable for some reason.
		QDir::setCurrent("/home/root/");
	}

	if (fileManagerRoot == MainWidget::FileManagerRootType::allFS) {
		mRootDirPath = QDir::rootPath();
	} else { // if (fileManagerRoot == MainWidget::FileManagerRootType::scriptsDir)
		mRootDirPath = trikKernel::Paths::userScriptsPath();
	}

	mDeleteAllFilesName = tr("Delete all...");
	mDeleteAllFilesPath = trikKernel::Paths::userScriptsPath() + mDeleteAllFilesName;
	QFile deleteAllFile(mDeleteAllFilesPath);
	/// This flag and operation is necessary to create file if it doesn't exists
	deleteAllFile.open(QIODevice::WriteOnly);


	mFileSystemModel.setIconProvider(mFileIconProvider.data());
	mFileSystemModel.setRootPath(mRootDirPath);
	mFileSystemModel.setFilter(QDir::AllEntries | QDir::Hidden | QDir::System | QDir::NoDot);

	connect(&mFileSystemModel, &QFileSystemModel::directoryLoaded, this, &FileManagerWidget::onDirectoryLoaded);

	mFilterProxyModel.setSourceModel(&mFileSystemModel);
	mFileSystemView.setModel(&mFilterProxyModel);

	mLayout.addWidget(&mCurrentPathLabel);
	mLayout.addWidget(&mFileSystemView);
	setLayout(&mLayout);

	mFileSystemView.setSelectionMode(QAbstractItemView::SingleSelection);
	mFileSystemView.setFocus();

	connect(mFileSystemView.selectionModel(), &QItemSelectionModel::currentChanged
			, this, &FileManagerWidget::onSelectionChanged);

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
	const QModelIndex &index = mFilterProxyModel.mapToSource(mFileSystemView.currentIndex());
	if (mFileSystemModel.isDir(index)) {
		if (QDir::setCurrent(mFileSystemModel.filePath(index))) {
			showCurrentDir();
		}
	} else {
		if (mFileSystemModel.fileName(index) == mDeleteAllFilesName) {
			removeAll();
		} else {
			mController.runFile(mFileSystemModel.filePath(index));
		}
	}
}

void FileManagerWidget::remove()
{
	const QModelIndex &index = mFilterProxyModel.mapToSource(mFileSystemView.currentIndex());
	if (!mFileSystemModel.isDir(index)) {
		if (mFileSystemModel.fileName(index) != mDeleteAllFilesName) {
			QMessageBox::StandardButton reply = QMessageBox::warning(this, tr("Confirm deletion")
					, tr("Are you sure you want to delete file?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
			if (reply == QMessageBox::Yes) {
				mFileSystemModel.remove(index);
			}
		}
	}
}

void FileManagerWidget::removeAll()
{
	QMessageBox::StandardButton reply = QMessageBox::warning(this, tr("Confirm deletion")
			, tr("Are you sure you want to delete all files?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No);
	if (reply == QMessageBox::Yes) {
		QDir dir(trikKernel::Paths::userScriptsPath());
		dir.setNameFilters({"*.js", "*.py"});
		dir.setFilter(QDir::Files);
		for (auto &&dirFile: dir.entryList()) {
			dir.remove(dirFile);
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
		case Qt::Key_Right: {
			remove();
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
	Q_UNUSED(previous)
	mLastSelectedFile = mFileSystemModel.filePath(mFilterProxyModel.mapToSource(current));
}

QString FileManagerWidget::currentPath()
{
	QString result = QDir(QDir::currentPath()).canonicalPath();

	if (mRootDirPath != "/") {
		/// @todo: fix this.
		const auto prefixLength = result.indexOf("scripts") + QString("scripts").length();
		result = result.replace(0, prefixLength, "");
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
	if (QFileInfo(QDir::currentPath()) == QFileInfo(mRootDirPath)) {
		filters |= QDir::NoDotDot;
	} else {
		filters &= ~QDir::NoDotDot;
	}

	filters &= ~QDir::Hidden;

	mFileSystemModel.setFilter(filters);

	mFileSystemView.setRootIndex(mFilterProxyModel.mapFromSource(
		mFileSystemModel.index(QDir::currentPath())));
	mFilterProxyModel.sort(0);
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
		currentIndex = mFilterProxyModel.mapFromSource(mFileSystemModel.index(mLastSelectedFile));
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

QIcon FileManagerWidget::LightFileIconProvider::icon(QFileIconProvider::IconType) const {
	return QIcon();
}

QIcon FileManagerWidget::LightFileIconProvider::icon(const QFileInfo &) const {
	return QIcon();
}

QString FileManagerWidget::LightFileIconProvider::type(const QFileInfo &) const {
	return QString();
}
