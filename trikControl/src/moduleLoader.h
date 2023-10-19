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
#include <QtCore/QSet>

#include <trikHal/systemConsoleInterface.h>

namespace trikControl {

/// Some devices like range sensor require kernel modules to work. This class keeps track of loaded kernel modules and
/// loads them if needed.
class ModuleLoader
{
public:
	/// Constructor.
	/// @param console - system console that is used to issue "modprobe" command.
	explicit ModuleLoader(trikHal::SystemConsoleInterface &console);

	/// Loads given module using modprobe. Returns true if module is loaded.
	bool load(const QString &module);

private:
	QSet<QString> mLoadedModules;
	trikHal::SystemConsoleInterface &mConsole;
};

}
