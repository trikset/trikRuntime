/* Copyright 2015 - 2016 CyberTech Labs Ltd.
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

#include "paths.h"

#include <QtCore/QCoreApplication>

#include "fileUtils.h"

#include <QDir>

using namespace trikKernel;

QString Paths::path(const Resource &resource)
{
	QDir appPath = QCoreApplication::applicationDirPath();  // ../bin/debug/myapp.app/Contents/MacOS/ or ../bin/debug
	if (appPath.dirName() == "MacOS") {
		appPath.cdUp();
		appPath.cdUp();
		appPath.cdUp();
	}

	const QString currentPath = FileUtils::normalizePath(appPath.path());

	if (resource == Resource::userScripts) {
		return currentPath + "scripts/";
	} else if (resource == Resource::translations) {
		return currentPath + "translations/";
	} else if (resource == Resource::images) {
		return currentPath + "images/";
	} else {
		return currentPath;
	}
}

QString Paths::configsPath()
{
	return path(Resource::configs);
}

QString Paths::logsPath()
{
	return path(Resource::logs);
}

QString Paths::mediaPath()
{
	return path(Resource::media);
}

QString Paths::userScriptsPath()
{
	return path(Resource::userScripts);
}

QString Paths::systemScriptsPath()
{
	return path(Resource::systemScripts);
}

QString Paths::coreDumpPath()
{
	return path(Resource::coreDump);
}

QString Paths::translationsPath()
{
	return path(Resource::translations);
}

QString Paths::trikRcName()
{
	return path(Resource::configs) + "stubTrikRc";
}

QString Paths::localSettings()
{
	return path(Resource::localSettings) + "localSettings.ini";
}

QString Paths::hostnameName()
{
	return path(Resource::configs) + "stubHostname";
}

QString Paths::imagesPath()
{
	return path(Resource::images);
}
