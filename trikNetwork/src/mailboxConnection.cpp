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

#include "src/mailboxConnection.h"

#include <QtCore/QStringList>

#include <QsLog.h>

using namespace trikNetwork;

MailboxConnection::MailboxConnection()
	: Connection(Protocol::messageLength, Heartbeat::use)
{
}

void MailboxConnection::connect(const QHostAddress &targetIp, int targetPort, int myServerPort, int myHullNumber)
{
	init(targetIp, targetPort);

	const QString handshake = QString("register:%1:%2").arg(myServerPort).arg(myHullNumber);
	send(handshake.toUtf8());
}

void MailboxConnection::sendConnectionInfo(const QHostAddress &ip, int port, int hullNumber)
{
	const QString info = QString("connection:%1:%2:%3").arg(ip.toString()).arg(port).arg(hullNumber);
	send(info.toUtf8());
}

void MailboxConnection::sendSelfInfo(int hullNumber)
{
	const QString info = QString("self:%3").arg(hullNumber);
	send(info.toUtf8());
}

void MailboxConnection::processData(const QByteArray &rawData)
{
	const QString data = QString::fromUtf8(rawData);
	const QString registerCommand = "register:";
	const QString connectionCommand = "connection:";
	const QString selfCommand = "self:";
	const QString dataCommand = "data:";
	const auto error = [](const QString &data) {
			QLOG_ERROR() << "Malformed data: " << data;
	};

	const auto parsedString = data.split(":");
	bool serverPortOk = false;
	bool hullNumberOk = false;
	if (data.startsWith(registerCommand)) {
		if (parsedString.size() != 3) {
			error(data);
		} else {
			const int serverPort = parsedString[1].toInt(&serverPortOk);
			const int hullNumber = parsedString[2].toInt(&hullNumberOk);
			if (!serverPortOk || !hullNumberOk) {
				error(data);
			} else {
				emit newConnection(peerAddress(), peerPort(), serverPort, hullNumber);
			}
		}
	} else if (data.startsWith(connectionCommand)) {
		if (parsedString.size() != 4) {
			error(data);
		} else {
			const int serverPort = parsedString[2].toInt(&serverPortOk);
			const int hullNumber = parsedString[3].toInt(&hullNumberOk);
			if (!serverPortOk || !hullNumberOk) {
				error(data);
			} else {
				emit connectionInfo(QHostAddress(parsedString[1]), serverPort, hullNumber);
			}
		}
	} else if (data.startsWith(selfCommand)) {
		if (parsedString.size() != 2) {
			error(data);
		} else {
			// Self-info. Host and port is to be determined automatically via socket.
			const int hullNumber = parsedString[1].toInt(&hullNumberOk);
			if (!hullNumberOk) {
				error(data);
			} else {
				emit connectionInfo(peerAddress(), peerPort(), hullNumber);
			}
		}
	} else if (data.startsWith(dataCommand)) {
		auto parsedString = data;
		parsedString.remove(0, dataCommand.length());
		emit newData(peerAddress(), peerPort(), parsedString.toUtf8());
	}
}
