#include "src/mailboxServer.h"

#include "src/mailboxConnection.h"

#include <QtNetwork/QNetworkInterface>
#include <QtCore/QSettings>

using namespace trikControl;

MailboxServer::MailboxServer(int port, QWaitCondition &receiveWaitCondition)
	: trikKernel::TrikServer([this] () { return connectionFactory(); })
	, mHullNumber(0)
	, mMyIp(determineMyIp())
	, mMyPort(port)
	, mReceiveWaitCondition(receiveWaitCondition)
{
	qRegisterMetaType<QHostAddress>("QHostAddress");

	if (mMyIp.isNull()) {
		qDebug() << "Self IP address is invalid, connect to a network and restart trikGui";
	}

	loadSettings();

	startServer(port);

	if (!mServerIp.isNull() && mServerIp != mMyIp) {
		connect(mServerIp, mServerPort);
	}
}

int MailboxServer::hullNumber() const
{
	return mHullNumber;
}

QHostAddress MailboxServer::serverIp()
{
	mAuxiliaryInformationLock.lockForRead();
	auto const result = mServerIp;
	mAuxiliaryInformationLock.unlock();
	return result;
}

QHostAddress MailboxServer::myIp()
{
	return mMyIp;
}

void MailboxServer::setHullNumber(int hullNumber)
{
	mHullNumber = hullNumber;
	saveSettings();

	forEveryConnection([this](trikKernel::Connection *connection) {
		QMetaObject::invokeMethod(connection, "sendConnectionInfo"
				, Q_ARG(QHostAddress const &, mMyIp)
				, Q_ARG(int, mMyPort)
				, Q_ARG(int, mHullNumber)
				);
	}
	, -1);
}

void MailboxServer::connect(QString const &ip, int port)
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

void MailboxServer::connect(QString const &ip)
{
	connect(ip, mMyPort);
}

trikKernel::Connection *MailboxServer::connect(QHostAddress const &ip, int port)
{
	auto const connection = new MailboxConnection();

	connectConnection(connection);

	startConnection(connection);

	QMetaObject::invokeMethod(connection, "connect"
			, Q_ARG(QHostAddress const &, ip)
			, Q_ARG(int, port)
			, Q_ARG(int, mMyPort)
			, Q_ARG(int, mHullNumber)
			);

	return connection;
}

trikKernel::Connection *MailboxServer::connectionFactory()
{
	auto connection = new MailboxConnection();

	QObject::connect(connection, SIGNAL(newConnection(QHostAddress, int, int, int))
			, this, SLOT(onNewConnection(QHostAddress, int, int, int)));

	connectConnection(connection);

	return connection;
}

void MailboxServer::connectConnection(trikKernel::Connection * connection)
{
	QObject::connect(connection, SIGNAL(connectionInfo(QHostAddress, int, int))
			, this, SLOT(onConnectionInfo(QHostAddress, int, int)));

	QObject::connect(connection, SIGNAL(newData(QHostAddress, int, QByteArray const &))
			, this, SLOT(onNewData(QHostAddress, int, QByteArray const &)));
}

QHostAddress MailboxServer::determineMyIp()
{
	/// @todo What if we are not in a network yet?
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

trikKernel::Connection *MailboxServer::prepareConnection(QHostAddress const &ip)
{
	// First, trying to reuse existing connection.
	auto const connectionObject = connection(ip);
	if (connectionObject != nullptr) {
		return connectionObject;
	}

	// Next, trying to create new connection to given IP. We need port, so checking if robot is known.
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

void MailboxServer::onNewConnection(QHostAddress const &ip, int clientPort, int serverPort, int hullNumber)
{
	if (ip == mMyIp) {
		/// Refuse to handle connections from ourselves.
		return;
	}

	mKnownRobotsLock.lockForRead();
	bool const knownRobot = mKnownRobots.contains(hullNumber, {ip, serverPort});
	auto const endpoints = mKnownRobots.values();
	mKnownRobotsLock.unlock();

	if (!knownRobot) {
		// Propagate information about newly connected robot through robot network.
		forEveryConnection([&ip, &serverPort, &hullNumber](trikKernel::Connection *connection) {
			QMetaObject::invokeMethod(connection, "sendConnectionInfo"
					, Q_ARG(QHostAddress const &, ip)
					, Q_ARG(int, serverPort)
					, Q_ARG(int, hullNumber)
					);
		}
		, hullNumber);
	}

	// Send known connection information to newly connected robot.
	auto const connectionObject = connection(ip, clientPort);
	if (connectionObject != nullptr) {
		mKnownRobotsLock.lockForRead();
		for (auto const &endpoint: endpoints) {
			QMetaObject::invokeMethod(connectionObject, "sendConnectionInfo"
					, Q_ARG(QHostAddress const &, endpoint.ip)
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
		qDebug() << "Something went wrong, new connection to" << ip << ":" << clientPort << "is dead";
		return;
	}

	if (!knownRobot) {
		mKnownRobotsLock.lockForWrite();
		mKnownRobots.insertMulti(hullNumber, {ip, serverPort});
		mKnownRobotsLock.unlock();
	}
}

void MailboxServer::send(int hullNumber, QString const &message)
{
	forEveryConnection([&message](trikKernel::Connection *connection) {
		auto const data = QString("data:%1").arg(message).toUtf8();
		QMetaObject::invokeMethod(connection, "send"
				, Q_ARG(QByteArray const &, data)
				);
	}
	, hullNumber);
}

void MailboxServer::send(QString const &message)
{
	send(-1, message);
}

void MailboxServer::onConnectionInfo(QHostAddress const &ip, int port, int hullNumber)
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

void MailboxServer::onNewData(QHostAddress const &ip, int port, QByteArray const &data)
{
	qDebug() << "New data received by a mailbox from " << ip << ":" << port << ", data is:" << data;

	int senderHullNumber = -1;
	mKnownRobotsLock.lockForRead();
	for (auto const &endpoint : mKnownRobots.values()) {
		if (endpoint == Endpoint{ip, port}) {
			senderHullNumber = mKnownRobots.key(endpoint);
		}
	}

	mKnownRobotsLock.unlock();

	if (senderHullNumber == -1) {
		qDebug() << "Received message from" << ip << ":" << port << "which is unknown at the moment";
	}

	mMessagesQueueLock.lockForWrite();
	mMessagesQueue.enqueue(data);
	mMessagesQueueLock.unlock();

	mReceiveWaitCondition.wakeOne();

	emit newMessage(senderHullNumber, QString(data));
}

bool MailboxServer::hasMessages()
{
	mMessagesQueueLock.lockForRead();
	bool const result = !mMessagesQueue.isEmpty();
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
	QSettings settings("localSettings.ini", QSettings::IniFormat);
	mHullNumber = settings.value("hullNumber", 0).toInt();
	mServerIp = QHostAddress(settings.value("server", mMyIp.toString()).toString());
	mServerPort = settings.value("serverPort", mMyPort).toInt();
	mAuxiliaryInformationLock.unlock();
}

void MailboxServer::saveSettings()
{
	mAuxiliaryInformationLock.lockForRead();
	QSettings settings("localSettings.ini", QSettings::IniFormat);
	settings.setValue("hullNumber", mHullNumber);
	settings.setValue("server", mServerIp.toString());
	settings.setValue("serverPort", mServerPort);
	mAuxiliaryInformationLock.unlock();
}

void MailboxServer::forEveryConnection(std::function<void(trikKernel::Connection *)> method, int hullNumber)
{
	mKnownRobotsLock.lockForRead();
	auto const endpoints = hullNumber == -1 ? mKnownRobots.values() : mKnownRobots.values(hullNumber);
	mKnownRobotsLock.unlock();

	for (auto const &endpoint : endpoints) {
		auto const connection = prepareConnection(endpoint.ip);
		if (connection == nullptr) {
			qDebug() << "Connection to" << endpoint.ip << ":" << endpoint.port << "is dead at the moment, message"
					<< "is not delivered. Will try to reestablish connection on next send.";
		} else {
			method(connection);
		}
	}
}
