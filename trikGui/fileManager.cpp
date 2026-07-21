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

#include "fileManager.h"

#include <QtCore/QDir>
#include <QtCore/QFile>
#include <QtCore/qglobal.h>

#include <trikKernel/paths.h>
#include <trikKernel/exceptions/trikRuntimeException.h>
#include <QsLog.h>

using namespace trikGui;

FileManager::FileManager(Controller &controller, SystemSettings::FileManagerRootType fileManagerRoot, QObject *parent)
	: QAbstractListModel(parent), mController(controller)
{
	QDir dir(trikKernel::Paths::userScriptsPath());

	if (!dir.exists()) {
		const bool result = dir.mkpath(trikKernel::Paths::userScriptsPath());
		if (!result) {
			QLOG_ERROR() << "Incorrect user scripts directory";
		}
	}

	if (fileManagerRoot == SystemSettings::FileManagerRootType::AllFS) {
		mRootDirPath = QDir::rootPath();
	} else {
		mRootDirPath = trikKernel::Paths::userScriptsPath();
	}

	// Always start browsing from the scripts directory, even in AllFS mode.
	mCurrentDir.setPath(trikKernel::Paths::userScriptsPath());
	connect(&mWatcher, &QFileSystemWatcher::directoryChanged, this, [this](const QString &) { loadCurrentDir(); });
	loadCurrentDir();
}

FileManager::~FileManager() {}

int FileManager::rowCount(const QModelIndex &parent) const
{
	if (parent.isValid()) {
		return 0;
	}
	return mEntries.count();
}

QVariant FileManager::data(const QModelIndex &index, int role) const
{
	if (!index.isValid() || index.row() >= mEntries.count()) {
		return QVariant();
	}

	const QFileInfo &info = mEntries.at(index.row());
	switch (role) {
	case FileNameRole:
		return info.fileName();
	case IsDirRole:
		return info.isDir();
	case FilePathRole:
		return info.absoluteFilePath();
	default:
		return QVariant();
	}
}

QHash<int, QByteArray> FileManager::roleNames() const
{
	QHash<int, QByteArray> roles;
	roles[FileNameRole] = "fileName";
	roles[IsDirRole] = "isDir";
	roles[FilePathRole] = "filePath";
	return roles;
}

void FileManager::open(int index)
{
	if (index < 0 || index >= mEntries.count()) {
		return;
	}

	const QFileInfo &info = mEntries.at(index);
	if (info.isDir()) {
		mCurrentDir.setPath(info.absoluteFilePath());
		loadCurrentDir();
	} else {
		mController.runFile(info.absoluteFilePath());
	}
}

void FileManager::remove(int index)
{
	if (index < 0 || index >= mEntries.count()) {
		return;
	}

	const QFileInfo &info = mEntries.at(index);
	if (!info.isDir()) {
		beginRemoveRows(QModelIndex(), index, index);
		QFile::remove(info.absoluteFilePath());
		mEntries.removeAt(index);
		endRemoveRows();
	}
}

void FileManager::removeAll()
{
	QDir dir(trikKernel::Paths::userScriptsPath());
	dir.setNameFilters({"*.js", "*.py"});
	dir.setFilter(QDir::Files);
	for (auto &&dirFile : dir.entryList()) {
		dir.remove(dirFile);
	}
	loadCurrentDir();
}

bool FileManager::isDir(int index) const
{
	if (index < 0 || index >= mEntries.count()) {
		return false;
	}
	return mEntries.at(index).isDir();
}

QString FileManager::currentPath() const
{
	QString result = mCurrentDir.canonicalPath();
	if (mRootDirPath != "/") {
		/// @todo: fix this.
		const auto prefixLength = result.indexOf("scripts") + QString("scripts").length();
		result = result.replace(0, prefixLength, "");
	}
	if (result.isEmpty()) {
		result = "/";
	} else if (result.count("/") > 2) {
		result = "/../" + result.section("/", result.count("/") - 1, result.count("/"));
	}

	return result;
}

void FileManager::loadCurrentDir()
{
	QDir::Filters filters = QDir::AllEntries | QDir::System | QDir::NoDot;
	if (QFileInfo(mCurrentDir.absolutePath()) == QFileInfo(mRootDirPath)) {
		filters |= QDir::NoDotDot;
	}

	mCurrentDir.setFilter(filters);
	mCurrentDir.setSorting(QDir::DirsFirst | QDir::Name);

	beginResetModel();
	mEntries = mCurrentDir.entryInfoList();
	endResetModel();

	// Update watcher to track the current directory
	if (!mWatcher.directories().isEmpty()) {
		mWatcher.removePaths(mWatcher.directories());
	}
	mWatcher.addPath(mCurrentDir.absolutePath());

	Q_EMIT currentPathChanged();
}

void FileManager::setQmlParent(QObject *parent) { setParent(parent); }
