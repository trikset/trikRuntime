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

#include "trikSystemConsole.h"

#include <QtCore/QString>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>

#include <QsLog.h>

using namespace trikHal::trik;

int TrikSystemConsole::system(const QString &command)
{
	return ::system(command.toStdString().c_str());
}

bool TrikSystemConsole::startProcess(const QString &processName, const QStringList &arguments)
{
	return QProcess::startDetached(processName, arguments);
}

bool TrikSystemConsole::startProcessSynchronously(const QString &processName, const QStringList &arguments
	, QString * const output)
{
	const QFileInfo executableFileInfo(processName);

	QProcess process;

	process.setWorkingDirectory(executableFileInfo.absolutePath());
	process.start(executableFileInfo.filePath(), arguments, QIODevice::ReadOnly | QIODevice::Unbuffered);

	process.waitForStarted();

	if (process.state() != QProcess::Running) {
		QLOG_ERROR() << "Cannot launch process" << executableFileInfo.filePath() << " in "
		             << executableFileInfo.absolutePath();

		return false;
	}

	if (!process.waitForFinished()) {
		QLOG_ERROR() << "Process" << executableFileInfo.filePath() << " in " <<
		executableFileInfo.absolutePath()
		             << "hanged up or finished unexpectedly!";

		return false;
	}

	if (output) {
		QString processOutput = process.readAllStandardOutput() + process.readAllStandardError();
		output->swap(processOutput);
	}

	return true;
}
