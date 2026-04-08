/* Copyright 2014 CyberTech Labs Ltd.
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

#pragma once

#include <QtCore/qglobal.h>

#include <QObject>

namespace trikGui {

/// Widget that shows some system settings.
class SystemSettings : public QObject
{
	Q_OBJECT
	Q_PROPERTY(FileManagerRootType fileManagerRootType READ fileManagerRootType WRITE setFileManagerRootType NOTIFY
			   fileManagerRootTypeChanged)

public:
	enum class FileManagerRootType { ScriptsDir, AllFS };
	Q_ENUM(FileManagerRootType)
	/// Constructor.
	/// @param parent - parent of this widget in terms of Qt widget parent/child system.
	/// @param fileManagerRoot - current FilesManagerRoot
	explicit SystemSettings(FileManagerRootType fileManagerRoot, QObject *parent = 0);

	/// Destructor.
	~SystemSettings();

	Q_INVOKABLE void setQmlParent(QObject *parent);

private:
	FileManagerRootType mFileManagerRootType;
	FileManagerRootType fileManagerRootType();
	void setFileManagerRootType(FileManagerRootType fileManagerRootType);
Q_SIGNALS:
	/// Emitted when we want to save current FilesManagerRoot
	void currentFilesDirPath(trikGui::SystemSettings::FileManagerRootType const &path);
	/// Emitted when root type changed (scripts is root dir or not)
	void fileManagerRootTypeChanged();
};

}
