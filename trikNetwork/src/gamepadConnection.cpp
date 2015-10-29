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

#include "src/gamepadConnection.h"

#include <QtCore/QStringList>

#include <QsLog.h>

using namespace trikNetwork;

GamepadConnection::GamepadConnection()
	: Connection(Protocol::endOfLineSeparator, Heartbeat::dontUse)
{
}

void GamepadConnection::processData(const QByteArray &data)
{
	const QString message(data);
	QStringList const cmd = message.split(" ", QString::SkipEmptyParts);
	const QString commandName = cmd.at(0).trimmed();
	if (commandName == "pad") {
		const int padId = cmd.at(1).trimmed().toInt();
		if (cmd.at(2).trimmed() == "up") {
			emit padUp(padId);
		} else {
			const int x = cmd.at(2).trimmed().toInt();
			const int y = cmd.at(3).trimmed().toInt();
			emit pad(padId, x, y);
		}
	} else if (commandName == "btn") {
		const int buttonCode = cmd.at(1).trimmed().toInt();
		emit button(buttonCode, 1);
	} else if (commandName == "wheel") {
		const int perc = cmd.at(1).trimmed().toInt();
		emit wheel(perc);
	} else {
		QLOG_ERROR() << "Gamepad: unknown command" << commandName;
	}
}

