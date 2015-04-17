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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtNetwork/QTcpSocket>

#include "connection.h"

namespace trikNetwork {

/// One connection to a client (or server) in mailbox. Supposed to be working in a separate thread.
class MailboxConnection : public Connection
{
	Q_OBJECT

public:
	MailboxConnection();

	/// Connect to given endpoint and send "register" command with our mailbox server port and hull number.
	Q_INVOKABLE void connect(const QHostAddress &targetIp, int targetPort, int myServerPort, int myHullNumber);

	/// Send info about other robot: its IP, port and hull number.
	Q_INVOKABLE void sendConnectionInfo(const QHostAddress &ip, int port, int hullNumber);

	/// Send our hull number. Used in response for connection request.
	Q_INVOKABLE void sendSelfInfo(int hullNumber);

signals:
	/// Emitted when "register" command is received.
	/// @param ip - remote robot IP.
	/// @param clientPort - remote robot client port (peer port), for reuse of this new connection.
	/// @param serverPort - remote robot mailbox server port.
	/// @param hullNumber - remote robot hull number.
	void newConnection(const QHostAddress &ip, int clientPort, int serverPort, int hullNumber);

	/// Emitted when remote robot sends info about other known robots ("connection" command).
	void connectionInfo(const QHostAddress &ip, int port, int hullNumber);

	/// Emitted when new data message received ("data" command).
	void newData(const QHostAddress &ip, int port, const QByteArray &data);

private:
	void processData(const QByteArray &data) override;
};

}
