/* Copyright 2014 - 2021 CyberTech Labs Ltd.
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
#include <QHostInfo>
#include <QsLog.h>

using namespace trikNetwork;

MailboxServer::MailboxServer(quint16 port)
	: TrikServer([this] () { return connectionFactory(); })
	, mHullNumber(0)
	, mMyIp(determineMyIp())
	, mMyPort(port)
{
	setObjectName("MailboxServer");
	qRegisterMetaType<QHostAddress>("QHostAddress");
	loadSettings();
}

bool MailboxServer::isConnected()
{
	return activeConnections() > 0;
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

void MailboxServer::renewIp()
{
	mMyIp = determineMyIp();
}

void MailboxServer::start()
{
	int i = 0;
	while (i < 5 && !startServer(mMyPort)) {
		mMyPort++;
		i++;
	}

	if (!mServerIp.isNull() && mMyIp == mSavedIp) {
		connectTo(mServerIp, mServerPort);
	}
}

void MailboxServer::setHullNumber(int hullNumber)
{
	mHullNumber = hullNumber;
	saveSettings();
	forEveryConnection(
		[this](MailboxConnection *c) {
			QMetaObject::invokeMethod(c, [c,this]() { c->sendConnectionInfo(mMyIp, mMyPort, mHullNumber); });
	}
	, -1);
}

void MailboxServer::connectTo(const QString &ip, int port)
{
	mAuxiliaryInformationLock.lockForRead();
	auto server = mServerIp;
	auto serverPort = mServerPort;
	mAuxiliaryInformationLock.unlock();

	if (server.toString() != ip || serverPort != port) {
		mAuxiliaryInformationLock.lockForWrite();
		mServerIp = QHostInfo::fromName(ip).addresses().first();
		mServerPort = port;
		mAuxiliaryInformationLock.unlock();

		saveSettings();
	}

	QReadLocker l(&mAuxiliaryInformationLock);
	connectTo(mServerIp, mServerPort);
}

void MailboxServer::connectTo(const QString &ip)
{
	connectTo(ip, mMyPort);
}

MailboxConnection *MailboxServer::connectTo(const QHostAddress &ip, int port)
{
	if (ip == mMyIp && port == mMyPort && isListening()) {
		// do not connect to self
		return nullptr;
	}

	if (auto connectionObject = connection(ip, port)) {
		return qobject_cast<MailboxConnection *>(connectionObject);
	}

	const auto c = connectionFactory();
	connect(this, &MailboxServer::startedConnection, c, [=]() {
		c->connect(ip, port, mMyPort, mHullNumber);
		disconnect(this, &MailboxServer::startedConnection, c, nullptr);
	});

	startConnection(c);
	return c;
}

MailboxConnection *MailboxServer::connectionFactory()
{
	auto connection = new MailboxConnection();

	QObject::connect(connection, &MailboxConnection::newConnection, this, &MailboxServer::onNewConnection);

	connectConnection(connection);

	return connection;
}

void MailboxServer::connectConnection(MailboxConnection * c)
{
	connect(c, &MailboxConnection::connectionInfo, this, [this](const QHostAddress &ip, int port, int hullNumber){
		onConnectionInfo(ip, port, hullNumber);
	});
	connect(c, &MailboxConnection::newData, this, &MailboxServer::onNewData);
}

QHostAddress MailboxServer::determineMyIp()
{
	QList<QNetworkInterface> ifs {
				// TRIK wlan0
				QNetworkInterface::interfaceFromName("wlan0")
				// Fallback to localhost
				, QNetworkInterface::interfaceFromName("lo")
				, QNetworkInterface::interfaceFromIndex(1)
	};
	for (auto &&interface : ifs) {
		if (interface.isValid()) {
			for (auto &&entry : interface.addressEntries()) {
				const QHostAddress ip = entry.ip();
				if (ip.protocol() == QAbstractSocket::IPv4Protocol) {
					return ip;
				}
			}
		}
	}

	return QHostAddress(); // Total fail
}

MailboxConnection *MailboxServer::prepareConnection(Endpoint &endpoint)
{
	// First, trying to reuse existing connection.
	if (endpoint.connectedPort != -1) {
		const auto connectionObject = connection(endpoint.ip, endpoint.connectedPort);
		if (connectionObject != nullptr) {
			return qobject_cast<MailboxConnection *>(connectionObject);
		}
	}

	return connectTo(endpoint.ip, endpoint.serverPort);
}

void MailboxServer::onNewConnection(const QHostAddress &ip, int clientPort, int serverPort, int hullNumber)
{
	if (ip == mMyIp && serverPort == mMyPort ) {
		/// Refuse to handle connections from ourselves.
		return;
	}

	mKnownRobotsLock.lockForRead();
	const auto knownRobot = mKnownRobots.contains(hullNumber, {ip, serverPort, clientPort});
	const auto endpoints = mKnownRobots.values();
	mKnownRobotsLock.unlock();

	if (!knownRobot) {
		// Propagate information about newly connected robot through robot network.
		forEveryConnection([ip, serverPort, hullNumber](MailboxConnection *c) {
			QMetaObject::invokeMethod(c, [=]() {c->sendConnectionInfo(ip, serverPort, hullNumber);});
		});
	}

	// Send known connection information to newly connected robot.
	const auto c = qobject_cast<MailboxConnection *>(connection(ip, clientPort));
	if (c != nullptr) {
		for (const auto &endpoint : endpoints) {
			mKnownRobotsLock.lockForRead();
			int endpointHullNumber = mKnownRobots.key(endpoint);
			mKnownRobotsLock.unlock();
			QMetaObject::invokeMethod(c, [c, endpoint, endpointHullNumber]() {
				c->sendConnectionInfo(endpoint.ip, endpoint.serverPort, endpointHullNumber);
			});
		}

		// Send information about myself.
		QMetaObject::invokeMethod(c, [this, c]() { c->sendSelfInfo(mHullNumber); });

		if (!knownRobot) {
			onConnectionInfo(ip, serverPort, hullNumber, clientPort);
		}
	} else {
		QLOG_ERROR() << "Something went wrong, new connection to" << ip << ":" << clientPort << "is dead";
		return;
	}
}

void MailboxServer::send(int hullNumber, const QString &message)
{
	const auto data = QString("data:%1").arg(message).toUtf8();
	forEveryConnection(
		[data](Connection *c) {
			QMetaObject::invokeMethod(c, [c, data]() { c->send(data); });
		}
	, hullNumber);
}

void MailboxServer::send(const QString &message)
{
	send(-1, message);
}

void MailboxServer::onConnectionInfo(const QHostAddress &ip, int serverPort, int hullNumber, int connectedPort)
{
	if (ip == mMyIp && serverPort == mMyPort) {
		return;
	}

	QVector<Endpoint> toDelete;
	mKnownRobotsLock.lockForRead();
	for (auto &&endpoint : mKnownRobots) {
		if (endpoint.ip == ip && endpoint.serverPort == serverPort) {
			// We want to remember connected robots but with a changed hull number.
			if (connectedPort == -1) {
				const auto connectionObject = connection(ip, endpoint.connectedPort);
				if (connectionObject != nullptr) {
					connectedPort = endpoint.connectedPort;
				}
			}
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

	mKnownRobots.insert(hullNumber, {ip, serverPort, connectedPort});
	mKnownRobotsLock.unlock();
}

void MailboxServer::onNewData(const QHostAddress &ip, int port, const QByteArray &data)
{
	QLOG_INFO() << "New data received by a mailbox from " << ip << ":" << port << ", data is:" << data;
	int senderHullNumber = -1;
	mKnownRobotsLock.lockForRead();
	for (const auto &endpoint : mKnownRobots) {
		if (endpoint.ip == ip && endpoint.connectedPort == port) {
			senderHullNumber = mKnownRobots.key(endpoint);
		}
	}

	mKnownRobotsLock.unlock();

	if (senderHullNumber == -1) {
		QLOG_INFO() << "Received message from" << ip << ":" << port << "which is unknown at the moment";
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

	return result;
}

bool MailboxServer::hasServer() const
{
	return isListening();
}

void MailboxServer::loadSettings()
{
	QWriteLocker l(&mAuxiliaryInformationLock);
	QSettings settings(trikKernel::Paths::localSettings(), QSettings::IniFormat);
	mHullNumber = settings.value("hullNumber", 0).toInt();
	auto tryResolve = [](const QString &ipOrName) {
		auto info = QHostInfo::fromName(ipOrName);
		if (info.error() != QHostInfo::NoError || info.addresses().isEmpty()) {
			QLOG_ERROR() << "Failed to get ip for " << ipOrName << ":" << info.error();
			return QHostAddress();
		} else {
			return info.addresses().first();
		}
	};

	mServerIp = tryResolve(settings.value("server", mMyIp.toString()).toString());
	mServerPort = settings.value("serverPort", mMyPort).toInt();
	mSavedIp = tryResolve(settings.value("localIp", mMyIp.toString()).toString());
}

void MailboxServer::saveSettings()
{
	mAuxiliaryInformationLock.lockForRead();
	QSettings settings(trikKernel::Paths::localSettings(), QSettings::IniFormat);
	settings.setValue("hullNumber", mHullNumber);
	settings.setValue("server", mServerIp.toString());
	settings.setValue("serverPort", mServerPort);
	settings.setValue("localIp", mMyIp.toString());
	mAuxiliaryInformationLock.unlock();
}

void MailboxServer::forEveryConnection(const std::function<void(MailboxConnection *)> &method, int hullNumber)
{
	mKnownRobotsLock.lockForRead();
	const auto keys = mKnownRobots.keys();
	mKnownRobotsLock.unlock();

	for (const auto key : keys) {
		if (hullNumber != -1 && key != hullNumber) {
			continue;
		}
		mKnownRobotsLock.lockForRead();
		auto endpoint = mKnownRobots.value(key);
		mKnownRobotsLock.unlock();

		const auto connection = prepareConnection(endpoint);
		if (connection == nullptr) {
			QLOG_ERROR() << "Connection to" << endpoint << "is dead at the moment, message"
					<< "is not delivered. Will try to reestablish connection on next send.";
		} else {
			onConnectionInfo(endpoint.ip, endpoint.serverPort, key, endpoint.serverPort);
			if (connection->isConnected()) {
				method(connection);
			} else {
				connect(connection, &Connection::connected, this, [method, connection](){method(connection);});
			}
		}
	}
}
