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
#include <QtNetwork/QHostAddress>

#include <trikKernel/trikServer.h>

#include "declSpec.h"

namespace trikControl {

/// Worker object for mailbox functionality. It is a server that is supposed to be run in a separate thread and
/// it allows to handle a number of connections, keeping them open if possible or attempting to reestablish them if
/// they errored.
///
/// Uses localSettings.ini settings file, keys:
/// hullNumber - hull number of this robot.
/// server - IP of a robot we last connected to.
/// serverPort - mailbox port of a robot we last connected to.
class MailboxServer : public trikKernel::TrikServer
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - a port for mailbox server.
	/// @param receiveWaitCondition - reference to a wait condition that is shared with "frontend" mailbox object
	///        and is used to block script execution on receive() call until new message arrives.
	MailboxServer(int port);

	/// Returns hull number of this robot.
	int hullNumber() const;

	/// Returns IP of leader robot to which we connected last.
	QHostAddress serverIp();

	/// Returns our own IP address or empty QHostAddress if we have none.
	QHostAddress myIp() const;

	/// Sets hull number for this robot and notifies all known robots about change.
	Q_INVOKABLE void setHullNumber(int hullNumber);

	/// Connects to robot by IP and port.
	Q_INVOKABLE void connect(QString const &ip, int port);

	/// Connects to robot by IP and uses port of local mailbox server as a port on remote robot.
	Q_INVOKABLE void connect(QString const &ip);

	/// Sends message to a robot with given hull number.
	Q_INVOKABLE void send(int hullNumber, QString const &message);

	/// Sends message to all known robots.
	Q_INVOKABLE void send(QString const &message);

	/// Returns true if there are incoming messages.
	Q_INVOKABLE bool hasMessages();

	/// Returns one incoming message or empty string if there are none.
	Q_INVOKABLE QString receive();

signals:
	/// Emitted when new message was received from a robot with given hull number.
	void newMessage(int senderHullNumber, QString const &message);

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

	void forEveryConnection(std::function<void(trikKernel::Connection *)> method, int hullNumber = -1);

	int mHullNumber;
	QHostAddress const mMyIp;
	int const mMyPort;
	QHostAddress mServerIp;
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

	QReadWriteLock mAuxiliaryInformationLock;
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
