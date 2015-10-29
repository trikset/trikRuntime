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

#include "trikServer.h"

#include "connection.h"

#include <QtCore/QDebug>

#include <QsLog.h>

using namespace trikNetwork;

TrikServer::TrikServer(const std::function<Connection *()> &connectionFactory)
	: mConnectionFactory(connectionFactory)
{
}

TrikServer::~TrikServer()
{
	for (QThread *thread : mConnections.keys()) {
		thread->quit();
		if (!thread->wait(1000)) {
			QLOG_ERROR() << "Unable to stop thread" << thread;
		}
	}

	qDeleteAll(mConnections);
	qDeleteAll(mConnections.keys());
}

void TrikServer::startServer(const int &port)
{
	if (!listen(QHostAddress::Any, port)) {
		QLOG_ERROR() << "Can not start server on port " << port;
	} else {
		QLOG_INFO() << "Server on port" << port << "started";
		qDebug() << "Server on port" << port << "started";
	}
}

void TrikServer::sendMessage(const QString &message)
{
	for (Connection * const connection : mConnections.values()) {
		connection->send(message.toUtf8());
	}
}

int TrikServer::activeConnections() const
{
	return mConnections.size();
}

void TrikServer::incomingConnection(qintptr socketDescriptor)
{
	QLOG_INFO() << "New connection, socket descriptor: " << socketDescriptor;

	Connection * const connectionWorker = mConnectionFactory();
	startConnection(connectionWorker);

	QMetaObject::invokeMethod(connectionWorker, "init", Q_ARG(int, socketDescriptor));
}

void TrikServer::startConnection(Connection * const connectionWorker)
{
	QThread * const connectionThread = new QThread();

	connect(connectionThread, SIGNAL(finished()), connectionThread, SLOT(deleteLater()));
	connect(connectionWorker, SIGNAL(disconnected(Connection*)), this, SLOT(onConnectionClosed(Connection *)));

	connectionWorker->moveToThread(connectionThread);

	const bool firstConnection = mConnections.isEmpty();
	mConnections.insert(connectionThread, connectionWorker);

	connectionThread->start();

	if (firstConnection) {
		/// @todo: Emit "connected" signal only when socket is actually connected.
		emit connected();
	}
}

Connection *TrikServer::connection(const QHostAddress &ip, int port) const
{
	for (auto *connection : mConnections.values()) {
		if (connection->peerAddress() == ip && connection->peerPort() == port) {
			return connection;
		}
	}

	return nullptr;
}

Connection *TrikServer::connection(const QHostAddress &ip) const
{
	for (auto *connection : mConnections.values()) {
		if (connection->peerAddress() == ip) {
			return connection;
		}
	}

	return nullptr;
}

void TrikServer::onConnectionClosed(Connection *connection)
{
	QThread * const thread = mConnections.key(connection);

	// Thread shall already be finished here.
	connection->deleteLater();

	mConnections.remove(thread);

	if (mConnections.isEmpty()) {
		emit disconnected();
	}
}
