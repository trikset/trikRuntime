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

#include "src/mailboxServer.h"

#include "src/mailboxConnection.h"

#include <QtNetwork/QNetworkInterface>
#include <QtCore/QSettings>

#include <trikKernel/paths.h>
#include <QsLog.h>

using namespace trikNetwork;

MailboxServer::MailboxServer(int port)
	: TrikServer([this] () { return connectionFactory(); })
	, mHullNumber(0)
	, mMyIp(determineMyIp())
	, mMyPort(port)
{
	setObjectName("MailboxServer");
	qRegisterMetaType<QHostAddress>("QHostAddress");

	if (mMyIp.isNull()) {
		QLOG_ERROR() << "Self IP address is invalid, connect to a network and restart trikGui";
	}

	loadSettings();
}

bool MailboxServer::isConnected()
{
	bool result = false;
	mKnownRobotsLock.lockForRead();
	for (const auto &endpoint : mKnownRobots.values()) {
		Connection * const connection = this->connection(endpoint.ip, endpoint.port);
		MailboxConnection * const mailboxConnection = dynamic_cast<MailboxConnection *>(connection);
		if (mailboxConnection && mailboxConnection->isConnected()) {
			result = true;
			break;
		}
	}

	mKnownRobotsLock.unlock();
	return result;
}

int MailboxServer::hullNumber() const
{
	return mHullNumber;
}

QHostAddress MailboxServer::serverIp()
{
	mAuxiliaryInformationLock.lockForRead();
	const auto result = mServerIp;
	mAuxiliaryInformationLock.unlock();
	return result;
}

QHostAddress MailboxServer::myIp() const
{
	return mMyIp;
}

void MailboxServer::start()
{
	startServer(mMyPort);

	if (!mServerIp.isNull() && mServerIp != mMyIp) {
		connect(mServerIp, mServerPort);
	}
}

void MailboxServer::setHullNumber(int hullNumber)
{
	mHullNumber = hullNumber;
	saveSettings();

	forEveryConnection([this](Connection *connection) {
		QMetaObject::invokeMethod(connection, "sendConnectionInfo"
				, Q_ARG(const QHostAddress &, mMyIp)
				, Q_ARG(int, mMyPort)
				, Q_ARG(int, mHullNumber)
				);
	}
	, -1);
}

void MailboxServer::connect(const QString &ip, int port)
{
	mAuxiliaryInformationLock.lockForRead();
	auto server = mServerIp;
	auto serverPort = mServerPort;
	mAuxiliaryInformationLock.unlock();

	if (server.toString() != ip || serverPort != port) {
		mAuxiliaryInformationLock.lockForWrite();
		mServerIp = QHostAddress(ip);
		mServerPort = port;
		mAuxiliaryInformationLock.unlock();

		saveSettings();
	}

	mAuxiliaryInformationLock.lockForRead();
	server = mServerIp;
	serverPort = mServerPort;
	mAuxiliaryInformationLock.unlock();

	if (server == mMyIp) {
		// Do not connect to ourselves.
		return;
	}

	connect(server, serverPort);
}

void MailboxServer::connect(const QString &ip)
{
	connect(ip, mMyPort);
}

Connection *MailboxServer::connect(const QHostAddress &ip, int port)
{
	const auto connection = new MailboxConnection();

	connectConnection(connection);

	startConnection(connection);

	QMetaObject::invokeMethod(connection, "connect"
			, Q_ARG(const QHostAddress &, ip)
			, Q_ARG(int, port)
			, Q_ARG(int, mMyPort)
			, Q_ARG(int, mHullNumber)
			);

	return connection;
}

Connection *MailboxServer::connectionFactory()
{
	auto connection = new MailboxConnection();

	QObject::connect(connection, SIGNAL(newConnection(QHostAddress, int, int, int))
			, this, SLOT(onNewConnection(QHostAddress, int, int, int)));

	connectConnection(connection);

	return connection;
}

void MailboxServer::connectConnection(Connection * connection)
{
	QObject::connect(connection, SIGNAL(connectionInfo(QHostAddress, int, int))
			, this, SLOT(onConnectionInfo(QHostAddress, int, int)));

	QObject::connect(connection, SIGNAL(newData(QHostAddress, int, const QByteArray &))
			, this, SLOT(onNewData(QHostAddress, int, const QByteArray &)));
}

QHostAddress MailboxServer::determineMyIp()
{
	/// @todo What if we are not in a network yet?
	QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
	for (const QNetworkInterface &interface : interfaces) {
		if (interface.name() == "wlan0") {
			const QList<QNetworkAddressEntry> entries = interface.addressEntries();
			for (const QNetworkAddressEntry &entry : entries) {
				const QHostAddress ip = entry.ip();
				if (ip.protocol() == QAbstractSocket::IPv4Protocol) {
					return ip;
				}
			}

			break;
		}
	}

	return QHostAddress();
}

Connection *MailboxServer::prepareConnection(const QHostAddress &ip)
{
	// First, trying to reuse existing connection.
	const auto connectionObject = connection(ip);
	if (connectionObject != nullptr) {
		return connectionObject;
	}

	// Next, trying to create new connection to given IP. We need port, so checking if robot is known.
	Endpoint targetEndpoint;
	mKnownRobotsLock.lockForRead();
	for (const auto &endpoint : mKnownRobots.values()) {
		if (endpoint.ip == ip) {
			targetEndpoint = endpoint;
			break;
		}
	}

	mKnownRobotsLock.unlock();

	if (targetEndpoint.ip.isNull()) {
		QLOG_ERROR() << "Trying to connect to unknown robot, IP:" << ip;
		return nullptr;
	}

	return connect(targetEndpoint.ip, targetEndpoint.port);
}

void MailboxServer::onNewConnection(const QHostAddress &ip, int clientPort, int serverPort, int hullNumber)
{
	if (ip == mMyIp) {
		/// Refuse to handle connections from ourselves.
		return;
	}

	mKnownRobotsLock.lockForRead();
	const bool knownRobot = mKnownRobots.contains(hullNumber, {ip, serverPort});
	const auto endpoints = mKnownRobots.values();
	mKnownRobotsLock.unlock();

	if (!knownRobot) {
		// Propagate information about newly connected robot through robot network.
		forEveryConnection([&ip, &serverPort, &hullNumber](Connection *connection) {
			QMetaObject::invokeMethod(connection, "sendConnectionInfo"
					, Q_ARG(const QHostAddress &, ip)
					, Q_ARG(int, serverPort)
					, Q_ARG(int, hullNumber)
					);
		}
		, hullNumber);
	}

	// Send known connection information to newly connected robot.
	const auto connectionObject = connection(ip, clientPort);
	if (connectionObject != nullptr) {
		mKnownRobotsLock.lockForRead();
		for (const auto &endpoint: endpoints) {
			QMetaObject::invokeMethod(connectionObject, "sendConnectionInfo"
					, Q_ARG(const QHostAddress &, endpoint.ip)
					, Q_ARG(int, endpoint.port)
					, Q_ARG(int, mKnownRobots.key(endpoint))
					);
		}

		// Send information about myself.
		QMetaObject::invokeMethod(connectionObject, "sendSelfInfo"
				, Q_ARG(int, mHullNumber)
				);

		mKnownRobotsLock.unlock();
	} else {
		QLOG_ERROR() << "Something went wrong, new connection to" << ip << ":" << clientPort << "is dead";
		return;
	}

	if (!knownRobot) {
		mKnownRobotsLock.lockForWrite();

		mKnownRobots.insertMulti(hullNumber, {ip, serverPort});
		mKnownRobotsLock.unlock();
	}
}

void MailboxServer::send(int hullNumber, const QString &message)
{
	forEveryConnection([&message](Connection *connection) {
		const auto data = QString("data:%1").arg(message).toUtf8();
		QMetaObject::invokeMethod(connection, "send"
				, Q_ARG(const QByteArray &, data)
				);
	}
	, hullNumber);
}

void MailboxServer::send(const QString &message)
{
	send(-1, message);
}

void MailboxServer::onConnectionInfo(const QHostAddress &ip, int port, int hullNumber)
{
	QList<Endpoint> toDelete;
	mKnownRobotsLock.lockForRead();
	for (const auto &endpoint : mKnownRobots.values()) {
		if (endpoint == Endpoint{ip, port}) {
			toDelete << endpoint;
		}
	}

	mKnownRobotsLock.unlock();

	mKnownRobotsLock.lockForWrite();
	for (const auto &endpoint : toDelete) {
		const auto keys = mKnownRobots.keys(endpoint);
		for (const auto &key : keys) {
			mKnownRobots.remove(key, endpoint);
		}
	}

	mKnownRobots.insertMulti(hullNumber, {ip, port});
	mKnownRobotsLock.unlock();
}

void MailboxServer::onNewData(const QHostAddress &ip, int port, const QByteArray &data)
{
	QLOG_INFO() << "New data received by a mailbox from " << ip << ":" << port << ", data is:" << data;
	qDebug() << "New data received by a mailbox from " << ip << ":" << port << ", data is:" << data;

	int senderHullNumber = -1;
	mKnownRobotsLock.lockForRead();
	for (const auto &endpoint : mKnownRobots.values()) {
		if (endpoint.ip == ip) {
			senderHullNumber = mKnownRobots.key(endpoint);
		}
	}

	mKnownRobotsLock.unlock();

	if (senderHullNumber == -1) {
		QLOG_INFO() << "Received message from" << ip << ":" << port << "which is unknown at the moment";
		qDebug() << "Received message from" << ip << ":" << port << "which is unknown at the moment";
	}

	mMessagesQueueLock.lockForWrite();
	mMessagesQueue.enqueue(data);
	mMessagesQueueLock.unlock();

	emit newMessage(senderHullNumber, QString(data));
}

bool MailboxServer::hasMessages()
{
	mMessagesQueueLock.lockForRead();
	const bool result = !mMessagesQueue.isEmpty();
	mMessagesQueueLock.unlock();

	return result;
}

QString MailboxServer::receive()
{
	mMessagesQueueLock.lockForWrite();
	QByteArray const result = !mMessagesQueue.isEmpty() ? mMessagesQueue.dequeue() : QByteArray();
	mMessagesQueueLock.unlock();

	return QString(result);
}

void MailboxServer::loadSettings()
{
	mAuxiliaryInformationLock.lockForWrite();
	QSettings settings(trikKernel::Paths::localSettings(), QSettings::IniFormat);
	mHullNumber = settings.value("hullNumber", 0).toInt();
	mServerIp = QHostAddress(settings.value("server", mMyIp.toString()).toString());
	mServerPort = settings.value("serverPort", mMyPort).toInt();
	mAuxiliaryInformationLock.unlock();
}

void MailboxServer::saveSettings()
{
	mAuxiliaryInformationLock.lockForRead();
	QSettings settings(trikKernel::Paths::localSettings(), QSettings::IniFormat);
	settings.setValue("hullNumber", mHullNumber);
	settings.setValue("server", mServerIp.toString());
	settings.setValue("serverPort", mServerPort);
	mAuxiliaryInformationLock.unlock();
}

void MailboxServer::forEveryConnection(std::function<void(Connection *)> method, int hullNumber)
{
	mKnownRobotsLock.lockForRead();
	const auto endpoints = hullNumber == -1 ? mKnownRobots.values() : mKnownRobots.values(hullNumber);
	mKnownRobotsLock.unlock();

	for (const auto &endpoint : endpoints) {
		const auto connection = prepareConnection(endpoint.ip);
		if (connection == nullptr) {
			QLOG_ERROR() << "Connection to" << endpoint.ip << ":" << endpoint.port << "is dead at the moment, message"
					<< "is not delivered. Will try to reestablish connection on next send.";
		} else {
			method(connection);
		}
	}
}
