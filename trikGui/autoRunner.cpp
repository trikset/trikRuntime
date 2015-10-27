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

#include "autoRunner.h"

#include <QtCore/QFileInfo>

#include <trikKernel/paths.h>
#include <QsLog.h>

#include "controller.h"

using namespace trikGui;

AutoRunner::AutoRunner(Controller &controller)
	: mController(controller)
{
	QFileInfo autorunFileInfo(fileName());
	if (autorunFileInfo.exists() && autorunFileInfo.isReadable()) {
		/// Timer is needed to wait for trikControl to initialize.
		/// @todo Make underlying components emit signal when they are ready for script execution.
		QTimer::singleShot(1000, this, SLOT(doLaunch()));
	}
}

void AutoRunner::doLaunch()
{
	QLOG_INFO() << "Launching autorun file:" << fileName();
	mController.runFile(fileName());
}

QString AutoRunner::fileName()
{
	return trikKernel::Paths::userScriptsPath() + "autorun.js";
}
