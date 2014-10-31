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

#pragma once

#include <QtCore/QObject>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QtCore/QScopedPointer>

namespace trikControl {

/// TCP server.
class TcpConnector : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - TCP port of a server.
	TcpConnector(int port);

signals:
	/// Emitted when there is incoming TCP message.
	void dataReady(QString const &message);
	void tcpDisconnectedSignal();

public slots:
	/// Starts a server and begins listening port for incoming connections.
	void startServer();

private slots:
	void tcpDisconnected();
	void connection();
	void networkRead();

private:
	int mPort;
	QScopedPointer<QTcpServer> mTcpServer;
	QScopedPointer<QTcpSocket> mTcpSocket;
};

}
