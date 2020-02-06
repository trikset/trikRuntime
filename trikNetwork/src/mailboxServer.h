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
#include <QtCore/QMultiHash>
#include <QtCore/QReadWriteLock>
#include <QtCore/QQueue>
#include <QtNetwork/QHostAddress>

#include "trikServer.h"

namespace trikNetwork {

/// Worker object for mailbox functionality. It is a server that is supposed to be run in a separate thread and
/// it allows to handle a number of connections, keeping them open if possible or attempting to reestablish them if
/// they errored.
///
/// Uses localSettings.ini settings file, keys:
/// hullNumber - hull number of this robot.
/// server - IP of a robot we last connected to.
/// serverPort - mailbox port of a robot we last connected to.
class MailboxServer : public TrikServer
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - a port for mailbox server.
	explicit MailboxServer(quint16 port);

	/// Returns true if at least one opened mailbox connection presents at the moment.
	bool isConnected();

	/// Returns hull number of this robot.
	int hullNumber() const;

	/// Returns IP of leader robot to which we connected last.
	QHostAddress serverIp();

	/// Returns our own IP address or empty QHostAddress if we have none.
	QHostAddress myIp() const;

	/// Tries to update our own IP address.
	void renewIp();

	/// Launches server, supposed to be called when mailbox server is already in a separate thread.
	Q_INVOKABLE void start();

	/// Sets hull number for this robot and notifies all known robots about change.
	Q_INVOKABLE void setHullNumber(int hullNumber);

	/// Connects to robot by IP and port.
	Q_INVOKABLE void connectTo(const QString &ip, int port);

	/// Connects to robot by IP and uses port of local mailbox server as a port on remote robot.
	Q_INVOKABLE void connectTo(const QString &ip);

	/// Sends message to a robot with given hull number.
	Q_INVOKABLE void send(int hullNumber, const QString &message);

	/// Sends message to all known robots.
	Q_INVOKABLE void send(const QString &message);

	/// Returns true if there are incoming messages.
	Q_INVOKABLE bool hasMessages();

	/// Returns one incoming message or empty string if there are none.
	Q_INVOKABLE QString receive();

signals:
	/// Emitted when new message was received from a robot with given hull number.
	void newMessage(int senderHullNumber, const QString &message);

private slots:
	void onNewConnection(const QHostAddress &ip, int clientPort, int serverPort, int hullNumber);
	void onConnectionInfo(const QHostAddress &ip, int port, int hullNumber);
	void onNewData(const QHostAddress &ip, int port, const QByteArray &data);

private:
	Connection *connectTo(const QHostAddress &ip, int port);

	Connection *connectionFactory();

	void connectConnection(Connection * connection);

	static QHostAddress determineMyIp();

	Connection *prepareConnection(const QHostAddress &ip);

	void loadSettings();
	void saveSettings();

	void forEveryConnection(std::function<void(Connection *)> method, int hullNumber = -1);

	int mHullNumber;
	QHostAddress mMyIp;
	QHostAddress mSavedIp;
	quint16 mMyPort;
	/// IP of leader robot to which we connected last
	QHostAddress mServerIp;
	int mServerPort{};

	struct Endpoint {
		QHostAddress ip;
		int port;
	};

	inline uint qHash(const Endpoint &key)
	{
		return ::qHash(key.ip.toString()) ^ static_cast<uint>(key.port);
	}

	friend bool operator ==(const MailboxServer::Endpoint &left, const MailboxServer::Endpoint &right);
	friend inline QDebug operator <<(QDebug dbg, const Endpoint &endpoint);

	QMultiHash<int, Endpoint> mKnownRobots;

	QQueue<QByteArray> mMessagesQueue;
	QReadWriteLock mMessagesQueueLock;
	QReadWriteLock mKnownRobotsLock;

	QReadWriteLock mAuxiliaryInformationLock;
};

inline bool operator ==(const MailboxServer::Endpoint &left, const MailboxServer::Endpoint &right)
{
	return left.ip == right.ip && left.port == right.port;
}

inline QDebug operator <<(QDebug dbg, const MailboxServer::Endpoint &endpoint)
{
	dbg.nospace() << endpoint.ip << ":" << endpoint.port;
	return dbg.space();
}

}
