/* Copyright 2014 - 2015 CyberTech Labs Ltd.
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

#include <trikKernel/fileUtils.h>
#include <trikScriptRunner/trikScriptRunner.h>

#include "src/connection.h"

#include <trikKernel/paths.h>
#include <QsLog.h>

using namespace trikCommunicator;

Connection::Connection(trikScriptRunner::TrikScriptRunner &trikScriptRunner, const QString &configVersion)
	: trikNetwork::Connection(trikNetwork::Protocol::messageLength, trikNetwork::Heartbeat::use)
	, mTrikScriptRunner(trikScriptRunner)
	, mConfigVersion(configVersion)
{
}

void Connection::processData(const QByteArray &data)
{
	QString command = QString::fromUtf8(data.data());

	if (!command.startsWith("keepalive")) {
		// Discard "keepalive" output.
		QLOG_INFO() << "Command: " << command;
	}

	if (command.startsWith("file:")) {
		command.remove(0, QString("file:").length());
		const int separatorPosition = command.indexOf(':');
		if (separatorPosition == -1) {
			QLOG_ERROR() << "Malformed 'file' command";
			return;
		}

		const QString fileName = command.left(separatorPosition);
		const QString fileContents = command.mid(separatorPosition + 1);
		trikKernel::FileUtils::writeToFile(fileName, fileContents, trikKernel::Paths::userScriptsPath());
		QMetaObject::invokeMethod(&mTrikScriptRunner, "brickBeep");
	} else if (command.startsWith("run:")) {
		command.remove(0, QString("run:").length());
		const QString fileContents = trikKernel::FileUtils::readFromFile(
				trikKernel::Paths::userScriptsPath() + command);

		QMetaObject::invokeMethod(&mTrikScriptRunner, [=](){mTrikScriptRunner.run(fileContents, command);});
	} else if (command == "stop") {
		emit stopCommandReceived();
	} else if (command.startsWith("direct:")) {
		command.remove(0, QString("direct:").length());
		QMetaObject::invokeMethod(&mTrikScriptRunner, [=](){mTrikScriptRunner.runDirectCommand(command);});
	} else if (command.startsWith("directScript:")) {
		command.remove(0, QString("directScript:").length());
		QMetaObject::invokeMethod(&mTrikScriptRunner, [=](){mTrikScriptRunner.run(command);});
	} else if (command == "configVersion") {
		send("configVersion: " + mConfigVersion.toUtf8());
	}
}
