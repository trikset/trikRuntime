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

#include "modeManager.h"
#include <QQmlContext>
#include <QSettings>
#include <trikKernel/paths.h>
using namespace trikKernel;
using namespace trikGui;

QString ModeManager::mCurrentMode;
ModeManager::ModeManager(QQmlApplicationEngine *engine, QObject *parent) : QObject(parent), mEngine(engine) {}
void ModeManager::initMode() {
	QSettings settings(trikKernel::Paths::localSettings(), QSettings::IniFormat);
	QString mode = settings.value("mode", "").toString();
	if (mode.isEmpty()) {
		settings.setValue("mode", "light");
		mode = "light";
	}
	ModeManager::mCurrentMode = mode;
}

void ModeManager::switchMode(QString mode) {
	if (ModeManager::mCurrentMode != mode) {
		QSettings settings(trikKernel::Paths::localSettings(), QSettings::IniFormat);
		settings.setValue("mode", mode);
		ModeManager::mCurrentMode = mode;
		mEngine->rootContext()->setContextProperty("ThemeMode", mode);
	}
}

void ModeManager::setQmlParent(QObject *parent) { setParent(parent); }

QString ModeManager::getCurrentMode() { return mCurrentMode; }
