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
#include "trikKernelDeclSpec.h"

namespace trikKernel {

/// Service that provides information about file paths used by system.
/// If there is "system-config.xml" file in a folder with launched binary, this service assumes that everything
/// is stored near binary (as in old "copy" deployment style), if not, it uses common Linux conventions for file
/// locations (configs in /etc/trik/trikRuntime/, media in /usr/share/trikRuntime/ and so on).
/// All methods returning paths return them ending with "/".
class TRIKKERNEL_EXPORT Paths
{
public:
	/// Path to a directory with trikControl config files.
	static QString configsPath();

	/// Where to put core dump if something goes wrong.
	static QString coreDumpPath();

	/// Where to put log files.
	static QString logsPath();

	/// Path to a directory containing "media" folder (with images and sounds).
	static QString mediaPath();

	/// Path to a directory with "system.js" file.
	static QString systemScriptsPath();

	/// Where to put script files sent by TRIK Studio and written by user.
	static QString userScriptsPath();

	/// Name of a directory containing "translations" folder.
	static QString translationsPath();

	/// Name (with path) of trikrc file.
	static QString trikRcName();

	/// Name (with path) of .ini file with local settings like locale and hull number.
	static QString localSettings();

	/// Name (with path) of hostname file.
	static QString hostnameName();

	/// Path to images obtained from the camera.
	static QString imagesPath();

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
		, images
	};

	static QString path(const Resource &resource);
};

}
