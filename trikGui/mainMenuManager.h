/* Copyright 2024 Daniel Chehade.
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
#include "controller.h"
#include "systemSettings.h"
#include <QObject>
#include <QQmlApplicationEngine>
#include <QtQml/qqml.h>

namespace trikGui {

/// Responsible for the display and operation of the main menu page
class MainMenuManager : public QObject
{
	Q_OBJECT
	QML_NAMED_ELEMENT(AppType)
	QML_UNCREATABLE("Use MainMenuManager singleton")
public:
	/// Constructor
	explicit MainMenuManager(const QString &configPath, QQmlApplicationEngine *engine, QObject *parent = nullptr);
	~MainMenuManager();
	enum class AppType {
		Files,
		Testing,
		Settings,
		Network,
		CommSettings,
		Language,
		SystemSettings,
		AppearanceMode,
		Information,
		Feedback
	};
	Q_ENUM(AppType)
	Q_INVOKABLE void createApp(AppType appType);

private Q_SLOTS:
	void changeFileManagerRoot(SystemSettings::FileManagerRootType const &path);

private:
	Controller mController;
	QQmlApplicationEngine *mEngine;
	SystemSettings::FileManagerRootType mFileManagerRoot; // current FilesManegerRoot

Q_SIGNALS:
	/// Emitted when root of file system changed
	void fileManagerRootChanged(SystemSettings::FileManagerRootType fileManagerRoot);
	void showPowerConfirm();
	void hidePowerConfirm();
};
} // namespace trikGui
