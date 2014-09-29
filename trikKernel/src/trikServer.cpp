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

#include "trikServer.h"

#include "trikKernel/connection.h"

#include <QtCore/QDebug>

using namespace trikKernel;

TrikServer::TrikServer(std::function<Connection *()> const &connectionFactory)
	: mConnectionFactory(connectionFactory)
{
}

TrikServer::~TrikServer()
{
	for (QThread *thread : mConnections.keys()) {
		thread->quit();
		if (!thread->wait(1000)) {
			qDebug() << "Unable to stop thread" << thread;
		}
	}

	qDeleteAll(mConnections);
	qDeleteAll(mConnections.keys());
}

void TrikServer::startServer(int const &port)
{
	if (!listen(QHostAddress::Any, port)) {
		qDebug() << "Can not start server on port " << port;
	} else {
		qDebug() << "Server on port" << port << "started";
	}
}

void TrikServer::sendMessage(QString const &message)
{
	for (Connection * const connection : mConnections.values()) {
		connection->send(message.toUtf8());
	}
}

void TrikServer::incomingConnection(qintptr socketDescriptor)
{
	qDebug() << "New connection, socket descriptor: " << socketDescriptor;

	Connection * const connectionWorker = mConnectionFactory();
	startConnection(connectionWorker);

	QMetaObject::invokeMethod(connectionWorker, "init", Q_ARG(int, socketDescriptor));
}

void TrikServer::startConnection(Connection * const connectionWorker)
{
	QThread * const connectionThread = new QThread();

	connect(connectionThread, SIGNAL(finished()), connectionThread, SLOT(deleteLater()));
	connect(connectionThread, SIGNAL(finished()), this, SLOT(onConnectionClosed()));

	connectionWorker->moveToThread(connectionThread);

	mConnections.insert(connectionThread, connectionWorker);

	connectionThread->start();
}

Connection *TrikServer::connection(QHostAddress const &ip, int port) const
{
	for (auto *connection : mConnections.values()) {
		if (connection->peerAddress() == ip && connection->peerPort() == port) {
			return connection;
		}
	}

	return nullptr;
}

Connection *TrikServer::connection(QHostAddress const &ip) const
{
	for (auto *connection : mConnections.values()) {
		if (connection->peerAddress() == ip) {
			return connection;
		}
	}

	return nullptr;
}

void TrikServer::onConnectionClosed()
{
	QThread * const thread = static_cast<QThread *>(sender());

	// Thread shall already be finished here.
	delete mConnections.value(thread);

	mConnections.remove(thread);
}
