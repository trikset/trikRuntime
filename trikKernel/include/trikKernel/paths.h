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

#pragma once

#include <QtCore/QString>

namespace trikKernel {

class Paths
{
public:
	static QString configsPath();
	static QString coreDumpPath();
	static QString logsPath();
	static QString mediaPath();
	static QString systemScriptsPath();
	static QString userScriptsPath();

	static QString userScriptsDirectoryName();
	static QString trikRcName();
	static QString localSettings();

private:
	enum class Resource {
		configs
		, translations
		, media
		, localSettings
		, logs
		, coreDump
		, userScripts
		, systemScripts
	};

	static QString path(const Resource resource);
};

}
