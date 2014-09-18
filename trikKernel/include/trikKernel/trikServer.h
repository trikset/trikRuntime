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

#pragma once

#include <functional>

#include <QtCore/QHash>
#include <QtCore/QThread>
#include <QtNetwork/QTcpServer>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
	typedef int qintptr;
#endif

namespace trikKernel {

class Connection;

/// Server that can handle multiple clients. Actual work is done in separate threads by Connection objects.
class TrikServer : public QTcpServer
{
	Q_OBJECT

public:
	explicit TrikServer(std::function<Connection *()> const &connectionFactory);

	~TrikServer() override;

	/// Starts listening given port on all network interfaces.
	void startServer(int const &port);

protected:
	void incomingConnection(qintptr socketDescriptor) override;

	/// Launches given connection in a separate thread. Takes ownership over connectionWorker object.
	void startConnection(Connection * const connectionWorker);

	Connection *connection(QHostAddress const &ip, int port) const;
	Connection *connection(QHostAddress const &ip) const;

private slots:
	/// Called when connection thread finishes.
	void onConnectionClosed();

private:
	/// Maps thread object to corresponding connection worker object, to be able to correctly stop and delete them all.
	QHash<QThread *, Connection *> mConnections;  // Has ownership over threads and connections.
	std::function<Connection *()> mConnectionFactory;
};

}
