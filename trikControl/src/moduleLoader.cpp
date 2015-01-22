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

#include "src/moduleLoader.h"

#include <QtCore/QDebug>

#include <QsLog.h>

using namespace trikControl;

/// Loads given module using modprobe.
bool ModuleLoader::load(QString const &module)
{
	if (mLoadedModules.contains(module)) {
		return true;
	}

	if (::system(QString("modprobe %1").arg(module).toStdString().c_str()) != 0) {
		QLOG_ERROR() << "modprobe" << module << "failed";
		return false;
	}

	QLOG_INFO() << "Module" << module << "loaded";

	mLoadedModules.insert(module);

	return true;
}
