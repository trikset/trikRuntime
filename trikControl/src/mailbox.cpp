#include "include/trikControl/mailbox.h"

#include "src/mailboxConnection.h"

#include <QtNetwork/QNetworkInterface>
#include <QtCore/QSettings>

using namespace trikControl;

Mailbox::Mailbox(int port)
	: trikKernel::TrikServer([this] () { return connectionFactory(); })
	, mHullNumber(0)
	, mMyIp(determineMyIp())
	, mMyPort(port)
{
	qRegisterMetaType<QHostAddress>("QHostAddress");

	if (mMyIp.isNull()) {
		qDebug() << "Something is wrong, self IP address is invalid";
	}

	loadSettings();

	startServer(port);

	if (!mServer.isNull() && mServer != mMyIp) {
		connect(mServer, mServerPort);
	}
}

void Mailbox::setHullNumber(int hullNumber)
{
	mHullNumber = hullNumber;
	saveSettings();
}

void Mailbox::connect(QString const &ip, int port)
{
	qDebug() << "Mailbox::connect, thread" << thread();

	if (mServer.toString() != ip || mServerPort != port) {
		mServer = QHostAddress(ip);
		mServerPort = port;

		saveSettings();
	}

	if (mServer == mMyIp) {
		// Do not connect to ourselves.
		return;
	}

	connect(mServer, mServerPort);
}

trikKernel::Connection *Mailbox::connect(QHostAddress const &ip, int port)
{
	auto const connection = new MailboxConnection();

	connectConnection(connection);

	startConnection(connection);

	QMetaObject::invokeMethod(connection, "onConnect"
			, Q_ARG(QHostAddress const &, ip)
			, Q_ARG(int, port)
			, Q_ARG(int, mMyPort)
			, Q_ARG(int, mHullNumber)
			);

	return connection;
}

trikKernel::Connection *Mailbox::connectionFactory()
{
	auto connection = new MailboxConnection();

	QObject::connect(connection, SIGNAL(newConnection(QHostAddress, int, int, int))
			, this, SLOT(onNewConnection(QHostAddress, int, int, int)));

	connectConnection(connection);

	return connection;
}

void Mailbox::connectConnection(trikKernel::Connection * connection)
{
	qDebug() << "Mailbox::connectConnection";

	QObject::connect(connection, SIGNAL(connectionInfo(QHostAddress, int, int))
			, this, SLOT(onConnectionInfo(QHostAddress, int, int)));

	QObject::connect(connection, SIGNAL(newData(QHostAddress, int, QByteArray const &))
			, this, SLOT(onNewData(QHostAddress, int, QByteArray const &)));
}

QHostAddress Mailbox::determineMyIp()
{
	QList<QNetworkInterface> interfaces = QNetworkInterface::allInterfaces();
	for (QNetworkInterface const &interface : interfaces) {
		if (interface.name() == "wlan0") {
			QList<QNetworkAddressEntry> const entries = interface.addressEntries();
			for (QNetworkAddressEntry const &entry : entries) {
				QHostAddress const ip = entry.ip();
				if (ip.protocol() == QAbstractSocket::IPv4Protocol) {
					return ip;
				}
			}

			break;
		}
	}

	return QHostAddress();
}

trikKernel::Connection *Mailbox::prepareConnection(QHostAddress const &ip)
{
	// First, trying to reuse existing connection.
	auto const connectionObject = connection(ip);
	if (connectionObject != nullptr) {
		return connectionObject;
	}

	Endpoint targetEndpoint;
	mKnownRobotsLock.lockForRead();
	for (auto const &endpoint : mKnownRobots.values()) {
		if (endpoint.ip == ip) {
			targetEndpoint = endpoint;
			break;
		}
	}
	mKnownRobotsLock.unlock();

	if (targetEndpoint.ip.isNull()) {
		qDebug() << "Trying to connect to unknown robot, IP:" << ip;
		return nullptr;
	}

	return connect(targetEndpoint.ip, targetEndpoint.port);
}

void Mailbox::onNewConnection(QHostAddress const &ip, int clientPort, int serverPort, int hullNumber)
{
	mKnownRobotsLock.lockForRead();
	bool const knownRobot = mKnownRobots.contains(hullNumber, {ip, serverPort});
	mKnownRobotsLock.unlock();

	if (!knownRobot) {
		// Propagate connection information through robots network.

		qDebug() << "Mailbox::onNewConnection" << mKnownRobots;

		mKnownRobotsLock.lockForRead();
		for (auto const &endpoint: mKnownRobots.values()) {
			auto const connectionObject = prepareConnection(endpoint.ip);
			if (connectionObject != nullptr) {
				QMetaObject::invokeMethod(connectionObject, "onConnectionInfo"
						, Q_ARG(QHostAddress const &, ip)
						, Q_ARG(int, serverPort)
						, Q_ARG(int, hullNumber)
						);
			}
		}

		// Send known connection information to newly connected robot.
		auto const connectionObject = connection(ip, clientPort);
		if (connectionObject != nullptr) {
			for (auto const &endpoint: mKnownRobots.values()) {
				QMetaObject::invokeMethod(connectionObject, "onConnectionInfo"
						, Q_ARG(QHostAddress const &, endpoint.ip)
						, Q_ARG(int, endpoint.port)
						, Q_ARG(int, mKnownRobots.key(endpoint))
						);
			}

			// Send information about myself.
			QMetaObject::invokeMethod(connectionObject, "onSelfInfo"
					, Q_ARG(int, mHullNumber)
					);
		} else {
			qDebug() << "Something went wrong, new connection to" << ip << ":" << clientPort << "is dead";
		}

		mKnownRobotsLock.unlock();

		mKnownRobotsLock.lockForWrite();
		mKnownRobots.insertMulti(hullNumber, {ip, serverPort});
		mKnownRobotsLock.unlock();
	}
}

void Mailbox::onConnectionInfo(QHostAddress const &ip, int port, int hullNumber)
{
	QList<Endpoint> toDelete;
	mKnownRobotsLock.lockForRead();
	for (auto const &endpoint : mKnownRobots.values()) {
		if (endpoint == Endpoint{ip, port}) {
			toDelete << endpoint;
		}
	}

	mKnownRobotsLock.unlock();

	mKnownRobotsLock.lockForWrite();
	for (auto const &endpoint : toDelete) {
		auto const keys = mKnownRobots.keys(endpoint);
		for (auto const &key : keys) {
			mKnownRobots.remove(key, endpoint);
		}
	}

	mKnownRobots.insertMulti(hullNumber, {ip, port});
	mKnownRobotsLock.unlock();
}

void Mailbox::send(int hullNumber, QVariant const &message)
{
	mKnownRobotsLock.lockForRead();
	auto const endpoints = mKnownRobots.values(hullNumber);
	mKnownRobotsLock.unlock();

	for (auto const &endpoint : endpoints) {
		auto const connectionObject = prepareConnection(endpoint.ip);
		if (connectionObject == nullptr) {
			/// @todo Connection dead, remove this robot from known.
			qDebug() << "Connection to" << endpoint.ip << ":" << endpoint.port << "is dead";
		} else {
			/// @todo Unicode serialization for strings.
			auto const data = QString("data:%1").arg(message.toString()).toLocal8Bit();
			QMetaObject::invokeMethod(connectionObject, "onSend"
					, Q_ARG(QByteArray const &, data)
					);
		}
	}
}

void Mailbox::onNewData(QHostAddress const &ip, int port, QByteArray const &data)
{
	qDebug() << "onNewData" << ip << ":" << port << ":" << data;

	Q_UNUSED(ip)
	Q_UNUSED(port)

	mMessagesQueueLock.lockForWrite();
	mMessagesQueue.enqueue(data);
	mMessagesQueueLock.unlock();
}

bool Mailbox::hasMessages()
{
	mMessagesQueueLock.lockForRead();
	bool const result = !mMessagesQueue.isEmpty();
	mMessagesQueueLock.unlock();

	return result;
}

QString Mailbox::receive()
{
	mMessagesQueueLock.lockForWrite();
	QByteArray const result = mMessagesQueue.dequeue();
	mMessagesQueueLock.unlock();

	return QString(result);
}

void Mailbox::loadSettings()
{
	QSettings settings("localSettings.ini", QSettings::IniFormat);
	mHullNumber = settings.value("hullNumber", 0).toInt();
	mServer = QHostAddress(settings.value("server", mMyIp.toString()).toString());
	mServerPort = settings.value("serverPort", mMyPort).toInt();
}

void Mailbox::saveSettings()
{
	QSettings settings("localSettings.ini", QSettings::IniFormat);
	settings.setValue("hullNumber", mHullNumber);
	settings.setValue("server", mServer.toString());
	settings.setValue("serverPort", mServerPort);
}
