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
#include <QObject>
#include <QQmlApplicationEngine>

namespace trikGui {

/// The widget that is responsible for switching the interface theme.
class ModeManager : public QObject
{
	Q_OBJECT
public:
	/// Constructor
	ModeManager(QQmlApplicationEngine *engine, QObject *parent);
	/// Reads settings and stores current mode. Does NOT touch the engine.
	/// Call engine->rootContext()->setContextProperty("ThemeMode", ModeManager::getCurrentMode()) after this.
	static void initMode();
	Q_INVOKABLE void switchMode(QString mode);
	Q_INVOKABLE void setQmlParent(QObject *parent);
	Q_INVOKABLE static QString getCurrentMode();

private:
	QQmlApplicationEngine *mEngine;
	static QString mCurrentMode;
};
} // namespace trikGui
