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
#include <QsLog.h>

using namespace trikNetwork;

TrikServer::TrikServer(const std::function<Connection *()> &connectionFactory)
	: mConnectionFactory(connectionFactory)
{
	qRegisterMetaType<qintptr>("qintptr");
	qRegisterMetaType<quint16>("quint16");
	connect(this, &TrikServer::startedConnection, this, [this](Connection *c) {
		const bool isFirstConnection = mConnections.isEmpty();
		mConnections.insert(c->thread(), c);
		if (isFirstConnection) {
			/// @todo: Emit "connected" signal only when socket is actually connected.
			emit connected();
		}
	});
}

TrikServer::~TrikServer()
{
	qDebug() << "TrikServer DELETED ALERT";
	for (QThread *thread : mConnections.keys()) {
		thread->quit();
		if (!thread->wait(1000)) {
			QLOG_ERROR() << "Unable to stop thread" << thread;
		}
	}

	qDeleteAll(mConnections.keyBegin(), mConnections.keyEnd());
}

void TrikServer::startServer(quint16 port)
{
//	if (!listen(QHostAddress::Any, port)) {
	if (!listen(QHostAddress::AnyIPv4, port)) {
		qDebug() << "MYLOG ERROR: Can not start server on port " << port;
		QLOG_ERROR() << "Can not start server on port " << port;
	} else {
		qDebug() << "MYLOG: Server on port" << port << "started";
		QLOG_INFO() << "Server on port" << port << "started";
	}
}

void TrikServer::sendMessage(const QString &message)
{
	for (auto *connection : mConnections) {
		QMetaObject::invokeMethod(connection, [=](){connection->send(message.toUtf8());});
	}
}

int TrikServer::activeConnections() const
{
	return mConnections.size();
}

void TrikServer::incomingConnection(qintptr socketDescriptor)
{
	qDebug() << "New incoming connection, socket descriptor: " << socketDescriptor;
	QLOG_INFO() << "New connection, socket descriptor: " << socketDescriptor;

	Connection * const connectionWorker = mConnectionFactory();
	startConnection(connectionWorker);

	QMetaObject::invokeMethod(connectionWorker, [=](){connectionWorker->init(socketDescriptor);});
}

void TrikServer::startConnection(Connection * const connectionWorker)
{
	qDebug() << "ALL CONNECTIONS" << __PRETTY_FUNCTION__;
	printAllConnections();
	auto connectionThread = new QThread(this);

	connectionWorker->moveToThread(connectionThread);

	connect(connectionThread, &QThread::finished, connectionWorker, &Connection::deleteLater);
	connect(connectionThread, &QThread::finished, connectionThread, &QThread::deleteLater);
	connect(connectionThread, &QThread::started, this, [this, connectionWorker]() {
		Q_EMIT startedConnection(connectionWorker);
	});

	connect(connectionWorker, &Connection::disconnected, this, &TrikServer::onConnectionClosed);
	connectionThread->start();
}

Connection *TrikServer::connection(const QHostAddress &ip, int port) const
{
	qDebug() << "ALL CONNECTIONS" << __PRETTY_FUNCTION__;
	printAllConnections();
	for (auto *connection : mConnections) {
		if (connection->isValid()) {
			if (connection->peerAddress() == ip && connection->peerPort() == port) {
				return connection;
			}
		} else {
			qDebug() << "MYLOG ERROR: Connection is not valid" << connection;
			QLOG_INFO() << "Connection is not valid" << connection;
		}
	}

	return nullptr;
}

Connection *TrikServer::connection(const QHostAddress &ip) const
{
	qDebug() << "ALL CONNECTIONS" << __PRETTY_FUNCTION__;
	printAllConnections();
	for (auto *connection : mConnections) {
		if (connection->isValid()) {
			if (connection->peerAddress() == ip) {
				return connection;
			}
		} else {
			qDebug() << "MYLOG ERROR: Connection is not valid" << connection;
			QLOG_INFO() << "Connection is not valid" << connection;
		}
	}

	return nullptr;
}

void TrikServer::onConnectionClosed(Connection *connection)
{
	const auto thread = mConnections.key(connection);
	qDebug() << "mConnections.remove" << connection->peerAddress() << connection->peerPort() << thread;

	mConnections.remove(thread);

	qDebug() << "ALL CONNECTIONS" << __PRETTY_FUNCTION__;
	printAllConnections();

	thread->quit();


	if (mConnections.isEmpty()) {
		emit disconnected();
	}
}

void TrikServer::printAllConnections() const
{
	for (auto *connection : mConnections) {
		connection->socketDescriptor();
	}
}
