/* Copyright 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include "stubSystemConsole.h"

#include <QtCore/QString>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>

#include <QsLog.h>

using namespace trikHal::stub;

int StubSystemConsole::system(const QString &command)
{
	QLOG_INFO() << "Calling stub system console with command:" << command;
	return 0;
}

bool StubSystemConsole::startProcess(const QString &processName, const QStringList &arguments)
{
	QLOG_INFO() << "Stub asked to start process" << processName << "with arguments" << arguments;
	return true;
}

bool StubSystemConsole::startProcessSynchronously(const QString &processName, const QStringList &arguments
	, QString * const output)
{
	QLOG_INFO() << "Stub asked to synchronously start process" << processName << "with arguments" << arguments;
	if (output) {
		*output = "";
	}

	return true;
}
