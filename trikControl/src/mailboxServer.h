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

#include <QtCore/QObject>
#include <QtCore/QMultiHash>
#include <QtCore/QReadWriteLock>
#include <QtCore/QQueue>
#include <QtCore/QWaitCondition>
#include <QtNetwork/QHostAddress>

#include <trikKernel/trikServer.h>

#include "declSpec.h"

namespace trikControl {

class MailboxServer : public trikKernel::TrikServer
{
	Q_OBJECT

public:
	MailboxServer(int port, QWaitCondition &receiveWaitCondition);

	Q_INVOKABLE void setHullNumber(int hullNumber);
	Q_INVOKABLE void connect(QString const &ip, int port);
	Q_INVOKABLE void send(int hullNumber, QVariant const &message);
	Q_INVOKABLE bool hasMessages();
	Q_INVOKABLE QString receive();

signals:
	void newMessage(QString const &message);

private slots:
	void onNewConnection(QHostAddress const &ip, int clientPort, int serverPort, int hullNumber);
	void onConnectionInfo(QHostAddress const &ip, int port, int hullNumber);
	void onNewData(QHostAddress const &ip, int port, QByteArray const &data);

private:
	trikKernel::Connection *connect(QHostAddress const &ip, int port);

	trikKernel::Connection *connectionFactory();

	void connectConnection(trikKernel::Connection * connection);

	static QHostAddress determineMyIp();

	trikKernel::Connection *prepareConnection(QHostAddress const &ip);

	void loadSettings();
	void saveSettings();

	int mHullNumber;
	QHostAddress const mMyIp;
	int const mMyPort;
	QHostAddress mServer;
	int mServerPort;

	struct Endpoint {
		QHostAddress ip;
		int port;
	};

	inline uint qHash(Endpoint const &key)
	{
		return ::qHash(key.ip.toString()) ^ key.port;
	}

	friend bool operator ==(MailboxServer::Endpoint const &left, MailboxServer::Endpoint const &right);
	friend inline QDebug operator <<(QDebug dbg, Endpoint const &endpoint);

	QMultiHash<int, Endpoint> mKnownRobots;

	QQueue<QByteArray> mMessagesQueue;
	QReadWriteLock mMessagesQueueLock;
	QReadWriteLock mKnownRobotsLock;

	QWaitCondition &mReceiveWaitCondition;
};

inline bool operator ==(MailboxServer::Endpoint const &left, MailboxServer::Endpoint const &right)
{
	return left.ip == right.ip && left.port == right.port;
}

inline QDebug operator <<(QDebug dbg, MailboxServer::Endpoint const &endpoint)
{
	dbg.nospace() << endpoint.ip << ":" << endpoint.port;
	return dbg.space();
}

}
