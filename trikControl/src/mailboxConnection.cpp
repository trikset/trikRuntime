/* Copyright 2014 Cybertech Labs Ltd.
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

#include <QtCore/QDebug>

#include <QsLog.h>

using namespace trikControl;

MailboxConnection::MailboxConnection()
	: trikKernel::Connection(trikKernel::Protocol::messageLength)
{
}

void MailboxConnection::connect(const QHostAddress &targetIp, int targetPort, int myServerPort, int myHullNumber)
{
	init(targetIp, targetPort);

	QString const handshake = QString("register:%1:%2").arg(myServerPort).arg(myHullNumber);
	send(handshake.toUtf8());
}

void MailboxConnection::sendConnectionInfo(QHostAddress const &ip, int port, int hullNumber)
{
	QString const info = QString("connection:%1:%2:%3").arg(ip.toString()).arg(port).arg(hullNumber);
	send(info.toUtf8());
}

void MailboxConnection::sendSelfInfo(int hullNumber)
{
	QString const info = QString("self:%3").arg(hullNumber);
	send(info.toUtf8());
}

void MailboxConnection::processData(QByteArray const &rawData)
{
	QString const data = QString::fromUtf8(rawData);
	QString const registerCommand = "register:";
	QString const connectionCommand = "connection:";
	QString const selfCommand = "self:";
	QString const dataCommand = "data:";
	auto const error = [](QString const &data) {
			QLOG_ERROR() << "Malformed data: " << data;
			qDebug() << "Malformed data: " << data;
	};

	if (data.startsWith(registerCommand)) {
		QStringList const parsedString = data.split(":");
		if (parsedString.size() != 3) {
			error(data);
		} else {
			bool serverPortOk = false;
			int const serverPort = parsedString[1].toInt(&serverPortOk);
			bool hullNumberOk = false;
			int const hullNumber = parsedString[2].toInt(&hullNumberOk);
			if (!serverPortOk || !hullNumberOk) {
				error(data);
			} else {
				emit newConnection(peerAddress(), peerPort(), serverPort, hullNumber);
			}
		}
	} else if (data.startsWith(connectionCommand)) {
		QStringList const parsedString = data.split(":");
		if (parsedString.size() != 4) {
			error(data);
		} else {
			bool serverPortOk = false;
			int const serverPort = parsedString[2].toInt(&serverPortOk);
			bool hullNumberOk = false;
			int const hullNumber = parsedString[3].toInt(&hullNumberOk);
			if (!serverPortOk || !hullNumberOk) {
				error(data);
			} else {
				emit connectionInfo(QHostAddress(parsedString[1]), serverPort, hullNumber);
			}
		}
	} else if (data.startsWith(selfCommand)) {
		QStringList const parsedString = data.split(":");
		if (parsedString.size() != 2) {
			error(data);
		} else {
			// Self-info. Host and port is to be determined automatically via socket.
			bool hullNumberOk = false;
			int const hullNumber = parsedString[1].toInt(&hullNumberOk);
			if (!hullNumberOk) {
				error(data);
			} else {
				emit connectionInfo(peerAddress(), peerPort(), hullNumber);
			}
		}
	} else if (data.startsWith(dataCommand)) {
		QString parsedString = data;
		parsedString.remove(0, dataCommand.length());
		emit newData(peerAddress(), peerPort(), parsedString.toUtf8());
	}
}
