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
	bool result = false;
	mKnownRobotsLock.lockForRead();
	for (auto &&endpoint : mKnownRobots) {
		const auto connection = this->connection(endpoint.ip, endpoint.port);
		auto mailboxConnection = qobject_cast<MailboxConnection *>(connection);
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

void MailboxServer::renewIp()
{
	mMyIp = determineMyIp();
}

void MailboxServer::start()
{
	startServer(mMyPort);

	if (!mServerIp.isNull() && mMyIp == mSavedIp) {
		connectTo(mServerIp, mServerPort);
	}
}

void MailboxServer::setHullNumber(int hullNumber)
{
	mHullNumber = hullNumber;
	saveSettings();
	forEveryConnection(
		[this](Connection *connection) {
			auto c = qobject_cast<MailboxConnection *>(connection);
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

Connection *MailboxServer::connectTo(const QHostAddress &ip, int port)
{
	// Why isListening !!!
	qDebug() << "connectTo" << ip << port << "MyData" << mMyIp << mMyPort << __PRETTY_FUNCTION__;
	//if (ip == mMyIp && port == mMyPort && isListening()) {
	if (ip == mMyIp && port == mMyPort) {
		// do not connect to self
		return nullptr;
	}

	qDebug() << "trying create new connection to" << ip << port << __PRETTY_FUNCTION__;
	const auto c = new MailboxConnection();
	connectConnection(c);
	connect(this, &MailboxServer::startedConnection, c, [=]() {
		c->connect(ip, port, mMyPort, mHullNumber);
	});
	startConnection(c);
	return c;
}

// X\Chomu tak? Why connection instead of MailboxConnection is there some problems !!!
Connection *MailboxServer::connectionFactory()
{
	auto connection = new MailboxConnection();

	QObject::connect(connection, &MailboxConnection::newConnection, this, &MailboxServer::onNewConnection);

	connectConnection(connection);

	return connection;
}

void MailboxServer::connectConnection(Connection * connection)
{
	auto c = qobject_cast<MailboxConnection *>(connection);
	connect(c, &MailboxConnection::connectionInfo, this, &MailboxServer::onConnectionInfo);
	connect(c, &MailboxConnection::newData, this, &MailboxServer::onNewData);
}

// Nado kakto vernut normal ip
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
			//QHostAddress("127.0.0.1"), QHostAddress("::1"), QHostAddress("192.168.1.114"), QHostAddress("192.168.77.211")
			for (auto &&ip : interface.allAddresses()) {
				//const QHostAddress ip = entry.ip();
				qDebug() << "IMPO try IP" << ip;
				if (ip.protocol() == QAbstractSocket::IPv4Protocol && ip != QHostAddress("127.0.0.1") && ip != QHostAddress("192.168.1.114")) {
					qDebug() << "IMPO My IP" << ip;
					return ip;
				}
			}
		}
	}

	qDebug() << "Impossible to determine my ip";
	return QHostAddress(); // Total fail
}

// Возможно здесь ошибка потому что у студии тот же ip что и рантайма?
Connection *MailboxServer::prepareConnection(const QHostAddress &ip, const int port)
{
	qDebug() << "Prepare:" << ip << ":" << port << __PRETTY_FUNCTION__;
	// First, trying to reuse existing connection.
	const auto connectionObject = connection(ip, port);
	if (connectionObject != nullptr) {
		return connectionObject;
	}
	qDebug() << "Connection isn't found:" << ip << ":" << port << __PRETTY_FUNCTION__;
	// Next, trying to create new connection to given IP. We need port, so checking if robot is known.
	Endpoint targetEndpoint;
	mKnownRobotsLock.lockForRead();
	for (auto &&endpoint : mKnownRobots) {
		if (endpoint.ip == ip && endpoint.port == port) {
			targetEndpoint = endpoint;
			break;
		}
	}

	mKnownRobotsLock.unlock();

	if (targetEndpoint.ip.isNull()) {
		qDebug() << "Trying to connect to unknown robot, IP:" << ip << port;
		QLOG_ERROR() << "Trying to connect to unknown robot, IP:" << ip << port;
		return nullptr;
	}

	return connectTo(targetEndpoint.ip, targetEndpoint.port);
}

void MailboxServer::onNewConnection(const QHostAddress &ip, int clientPort, int serverPort, int hullNumber)
{
	qDebug() << "IP" << ip << "ClientPort" << clientPort << "ServerPort" << serverPort << "Hull" << hullNumber << __PRETTY_FUNCTION__;
	if (ip == mMyIp && serverPort == mMyPort ) {
		/// Refuse to handle connections from ourselves.
		return;
	}

	mKnownRobotsLock.lockForRead();
	const auto knownRobot = mKnownRobots.contains(hullNumber, {ip, serverPort});
	const auto endpoints = mKnownRobots.values();
	mKnownRobotsLock.unlock();

	qDebug() << "is knownRobot:" << ip << ":" << serverPort << __PRETTY_FUNCTION__;
	if (!knownRobot) {
		qDebug() << "Robot unknown:" << ip << ":" << serverPort << __PRETTY_FUNCTION__;
		// Propagate information about newly connected robot through robot network.
		forEveryConnection([ip, serverPort, hullNumber](Connection *connection) {
			auto c = qobject_cast<MailboxConnection *>(connection);
			QMetaObject::invokeMethod(c, [=]() {c->sendConnectionInfo(ip, serverPort, hullNumber);});
		});
	}

	// Send known connection information to newly connected robot.
	const auto c = qobject_cast<MailboxConnection *>(connection(ip, clientPort));
	if (c != nullptr) {
		qDebug() << "connection happens:" << ip << ":" << clientPort << __PRETTY_FUNCTION__;
		mKnownRobotsLock.lockForRead();
		for (const auto &endpoint : endpoints) {
			QMetaObject::invokeMethod(c, [this, c, endpoint]() {
				// This function can be slow (linear time), because QHash's internal data structure is optimized for
				// fast lookup by key, not by value.
				c->sendConnectionInfo(endpoint.ip, endpoint.port, mKnownRobots.key(endpoint));
			});
		}

		// Send information about myself.
		QMetaObject::invokeMethod(c, [this, c]() { c->sendSelfInfo(mHullNumber); });
		mKnownRobotsLock.unlock();
	} else {
		qDebug() << "MYLOG ERROR: Something went wrong, new connection to" << ip << ":" << clientPort << "is dead";
		QLOG_ERROR() << "Something went wrong, new connection to" << ip << ":" << clientPort << "is dead";
		return;
	}

	if (!knownRobot) {
		mKnownRobotsLock.lockForWrite();
		qDebug() << "Instert Robot:" << ip << ":" << serverPort << __PRETTY_FUNCTION__;
		mKnownRobots.insert(hullNumber, {ip, serverPort});
		mKnownRobotsLock.unlock();
	}
}

void MailboxServer::send(int hullNumber, const QString &message)
{
	qDebug() << "Robot is asking for send" << message;
	const auto data = QString("data:%1").arg(message).toUtf8();
	forEveryConnection(
		[data](Connection *c) {
			//Q_UNUSED(c)
			QMetaObject::invokeMethod(c, [c, data]() { c->send(data); });
		}
	, hullNumber);
}

void MailboxServer::send(const QString &message)
{
	send(-1, message);
}

void MailboxServer::onConnectionInfo(const QHostAddress &ip, int port, int hullNumber)
{
	if (ip == mMyIp && port == mMyPort) {
		return;
	}

	QVector<Endpoint> toDelete;
	mKnownRobotsLock.lockForRead();
	qDebug() << "Known robots started:" << __PRETTY_FUNCTION__;
	for (const auto &endpoint : mKnownRobots) {
		qDebug() << endpoint;
	}
	for (auto &&endpoint : mKnownRobots) {
		if (endpoint == Endpoint{ip, port}) {
			toDelete << endpoint;
		}
	}

//	mKnownRobotsLock.unlock();

//	mKnownRobotsLock.lockForWrite();
	for (const auto &endpoint : toDelete) {
		const auto keys = mKnownRobots.keys(endpoint);
		for (const auto &key : keys) {
			mKnownRobots.remove(key, endpoint);
		}
	}
	qDebug() << "mKnownRobots inserts:" << hullNumber << ip << ":" << port << __PRETTY_FUNCTION__;
	mKnownRobots.insert(hullNumber, {ip, port});
	qDebug() << "Known robots ended:" << __PRETTY_FUNCTION__;
	for (const auto &endpoint : mKnownRobots) {
		qDebug() << "	" << endpoint;
	}
	mKnownRobotsLock.unlock();
}
// !!!! ДВА КОНЕКШОН ИНФА С РЕГИСТРА
void MailboxServer::onNewData(const QHostAddress &ip, int port, const QByteArray &data)
{
	QLOG_INFO() << "New data received by a mailbox from " << ip << ":" << port << ", data is:" << data;
	int senderHullNumber = -1;
	mKnownRobotsLock.lockForRead();
	for (const auto &endpoint : mKnownRobots) {
		if (endpoint.ip == ip) {
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

void MailboxServer::forEveryConnection(const std::function<void(Connection *)> &method, int hullNumber)
{
	mKnownRobotsLock.lockForRead();
	qDebug() << "Known robots:";
	for (const auto &endpoint : mKnownRobots) {
		qDebug() << endpoint;
	}
	const auto endpoints = hullNumber == -1 ? mKnownRobots.values() : mKnownRobots.values(hullNumber);
	mKnownRobotsLock.unlock();

	for (const auto &endpoint : endpoints) {
		const auto connection = prepareConnection(endpoint.ip, endpoint.port);
		if (connection == nullptr) {
			QLOG_ERROR() << "Connection to" << endpoint.ip << ":" << endpoint.port << "is dead at the moment, message"
					<< "is not delivered. Will try to reestablish connection on next send.";
		} else {
			if (connection->isConnected()) {
				method(connection);
			} else {
				connect(connection, &Connection::connected, this, method);
			}
		}
	}
}
