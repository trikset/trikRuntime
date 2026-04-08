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

#include "systemSettings.h"
#include <QQmlEngine>

using namespace trikGui;

SystemSettings::SystemSettings(FileManagerRootType fileManagerRoot, QObject *parent)
	: QObject(parent), mFileManagerRootType(fileManagerRoot) {
	qmlRegisterUncreatableType<SystemSettings>("SystemSettings", 1, 0, "RootType", "Enum is not a type");
}

SystemSettings::~SystemSettings() {}
SystemSettings::FileManagerRootType SystemSettings::fileManagerRootType() { return mFileManagerRootType; }

void SystemSettings::setFileManagerRootType(FileManagerRootType fileManagerRootType) {
	mFileManagerRootType = fileManagerRootType;
	Q_EMIT fileManagerRootTypeChanged();
	Q_EMIT currentFilesDirPath(fileManagerRootType);
}

void SystemSettings::setQmlParent(QObject *parent) { setParent(parent); }
