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
 * limitations under the License. */

/* This file was modified by Yurii Litvinov to make it comply with the requirements of trikRuntime
 * project. See git revision history for detailed changes. */

#pragma once

#include <QtCore/qglobal.h>
#include <QAbstractListModel>
#include <QDir>
#include <QFileInfo>
#include <QFileSystemWatcher>

#include "controller.h"
#include "systemSettings.h"

namespace trikGui {

/// File system browser able to launch executable files and .qts scripts.
class FileManager : public QAbstractListModel
{
	Q_OBJECT

	Q_PROPERTY(QString currentPath READ currentPath NOTIFY currentPathChanged)

public:
	enum Roles {
		FileNameRole = Qt::UserRole + 1,
		IsDirRole,
		FilePathRole
	};
	Q_ENUM(Roles)

	/// Constructor
	/// @param controller - reference to controller object which provides access to low-level functionality.
	/// @param fileManagerRoot - root directory for file browser.
	/// @param parent - parent of this widget in Qt object hierarchy.
	explicit FileManager(Controller &controller, SystemSettings::FileManagerRootType fileManagerRoot
			, QObject *parent = nullptr);

	~FileManager() override;

	// QAbstractListModel overrides
	int rowCount(const QModelIndex &parent = QModelIndex()) const override;
	QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
	QHash<int, QByteArray> roleNames() const override;

	/// Open file or enter directory by list index
	Q_INVOKABLE void open(int index);
	/// Remove file by list index
	Q_INVOKABLE void remove(int index);
	/// Remove all script files
	Q_INVOKABLE void removeAll();
	/// Check whether entry at index is a directory
	Q_INVOKABLE bool isDir(int index) const;
	/// Set a QML component as a parent for a given QObject
	Q_INVOKABLE void setQmlParent(QObject *parent);

Q_SIGNALS:
	void currentPathChanged();

private:
	void loadCurrentDir();
	QString currentPath() const;

	Controller &mController;
	QString mRootDirPath;
	QDir mCurrentDir;
	QList<QFileInfo> mEntries;
	QFileSystemWatcher mWatcher;
};

}
