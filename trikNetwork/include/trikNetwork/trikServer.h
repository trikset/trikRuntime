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

#include <functional>

#include <QtCore/QHash>
#include <QtCore/QThread>
#include <QtNetwork/QTcpServer>
#include <QtCore/QTimer>

#include "declSpec.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	typedef int qintptr;
#endif

namespace trikNetwork {

class Connection;

/// Server that can handle multiple clients. Actual work is done in separate threads by Connection objects.
class TRIKNETWORK_EXPORT TrikServer : public QTcpServer
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param connectionFactory - function that provides actual connection objects.
	explicit TrikServer(const std::function<Connection *()> &connectionFactory);

	~TrikServer() override;

	/// Returns number of connections currently opened.
	int activeConnections() const;

	/// Starts listening given port on all network interfaces.
	Q_INVOKABLE void startServer(const int &port);

public slots:
	/// Broadcasts message across all opened connections.
	void sendMessage(const QString &message);

signals:
	/// Emitted when we get the first connection established.
	void connected();

	/// Emitted when the last connection closes.
	void disconnected();

protected:
	void incomingConnection(qintptr socketDescriptor) override;

	/// Launches given connection in a separate thread. Takes ownership over connectionWorker object.
	void startConnection(Connection * const connectionWorker);

	/// Searches connection to given IP and port in a list of all open connections. Note that if connection is added
	/// by startConnection() call but not finished to open yet, it will not be found.
	Connection *connection(const QHostAddress &ip, int port) const;

	/// Searches connection to given IP and any port in a list of all open connections. Will return arbitrary matching
	/// connection if there are more than one connection with this IP. Note that if connection is added
	/// by startConnection() call but not finished to open yet, it will not be found.
	Connection *connection(const QHostAddress &ip) const;

private slots:
	/// Called when connection thread finishes.
	void onConnectionClosed(Connection *connection);

private:
	/// Maps thread object to corresponding connection worker object, to be able to correctly stop and delete them all.
	QHash<QThread *, Connection *> mConnections;  // Has ownership over threads and connections.

	/// Function that provides actual connection objects.
	std::function<Connection *()> mConnectionFactory;
};

}
