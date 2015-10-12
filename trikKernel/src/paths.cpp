/* Copyright 2015 CyberTech Labs Ltd.
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
#include <QtCore/QFileInfo>

#include "fileUtils.h"

using namespace trikKernel;

QString Paths::path(const Resource resource)
{
	QString defaultPath;
	const QString currentPath = FileUtils::normalizePath(QCoreApplication::applicationDirPath());

	switch (resource) {
	case Resource::configs:
		defaultPath = "/etc/trik/trikRuntime/";
		break;
	case Resource::coreDump:
		defaultPath = ".";
		break;
	case Resource::localSettings:
		defaultPath = "/home/root/.config/trikRuntime/";
		break;
	case Resource::logs:
		defaultPath = "/var/log/trikRuntime/";
		break;
	case Resource::media:
		defaultPath = "/usr/share/trikRuntime/";
		break;
	case Resource::systemScripts:
		defaultPath = "/usr/share/trikRuntime/";
		break;
	case Resource::translations:
		defaultPath = "/usr/share/trikRuntime/";
		break;
	case Resource::userScripts:
		defaultPath = "/home/root/";
		break;
	}

	QFileInfo fileInfo(currentPath + "/system-config.xml");
	if (fileInfo.exists()) {
		return currentPath;
	} else {
		return defaultPath;
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

QString Paths::userScriptsDirectoryName()
{
	return "scripts/";
}

QString Paths::trikRcName()
{
	return "/etc/trik/trikrc";
}

QString Paths::localSettings()
{
	return path(Resource::localSettings) + "localSettings.ini";
}

QString Paths::hostnameName()
{
	return "/etc/hostname";
}
