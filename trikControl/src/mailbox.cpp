#include "include/trikControl/mailbox.h"

#include "src/mailboxConnection.h"

using namespace trikControl;

Mailbox::Mailbox(int port, int hullNumber)
	: trikKernel::TrikServer([this] () { return connectionFactory(); })
	, mHullNumber(hullNumber)
	, mPort(port)
{
	qRegisterMetaType<QHostAddress>("QHostAddress");

	startServer(port);
}

void Mailbox::setHullNumber(int hullNumber)
{
	mHullNumber = hullNumber;
}

void Mailbox::connect(QString const &ip, int port)
{
	auto const connection = new MailboxConnection();

	connectConnection(connection);

	startConnection(connection);

	QMetaObject::invokeMethod(connection, "onConnect"
			, Q_ARG(QString const &, ip)
			, Q_ARG(int, port)
			, Q_ARG(int, mHullNumber)
			);
}

trikKernel::Connection *Mailbox::connectionFactory()
{
	auto connection = new MailboxConnection();

	QObject::connect(connection, SIGNAL(newConnection(QHostAddress, int, int))
			, this, SLOT(onNewConnection(QHostAddress, int, int)));

	connectConnection(connection);

	return connection;
}

void Mailbox::connectConnection(trikKernel::Connection * connection)
{
	QObject::connect(connection, SIGNAL(updateConnectionInfo(QHostAddress, int, int))
			, this, SLOT(onUpdateConnectionInfo(QHostAddress, int, int)));

	QObject::connect(connection, SIGNAL(newData(QHostAddress, int, QByteArray const &))
			, this, SLOT(onNewData(QHostAddress, int, QByteArray const &)));
}

void Mailbox::onNewConnection(QHostAddress const &ip, int port, int hullNumber)
{
	if (!mKnownRobots.contains(hullNumber, {ip, port})) {
		// Propagate connection information through robots network.

		qDebug() << "Mailbox::onNewConnection" << mKnownRobots;

		for (auto const &endpoint: mKnownRobots.values()) {
			auto const connectionObject = connection(endpoint.ip, endpoint.port);
			if (connectionObject == nullptr) {
				/// @todo Connection dead, remove this robot from known.
				qDebug() << "Connection to" << endpoint.ip << ":" << endpoint.port << "is dead";
			} else {
				QMetaObject::invokeMethod(connectionObject, "onUpdateConnectionInfo"
						, Q_ARG(QHostAddress const &, ip)
						, Q_ARG(int, port)
						, Q_ARG(int, hullNumber)
						);
			}
		}

		// Send known connection information to newly connected robot.
		auto const connectionObject = connection(ip, port);
		if (connectionObject != nullptr) {
			for (auto const &endpoint: mKnownRobots.values()) {
				QMetaObject::invokeMethod(connectionObject, "onUpdateConnectionInfo"
						, Q_ARG(QHostAddress const &, endpoint.ip)
						, Q_ARG(int, endpoint.port)
						, Q_ARG(int, mKnownRobots.key(endpoint))
						);
			}

			// Send information about myself.
			QMetaObject::invokeMethod(connectionObject, "onUpdateConnectionInfo"
					, Q_ARG(QHostAddress const &, QHostAddress())
					, Q_ARG(int, -1)
					, Q_ARG(int, mHullNumber)
					);
		} else {
			qDebug() << "Something went wrong, new connection to" << ip << ":" << port << "is dead";
		}

		mKnownRobots.insertMulti(hullNumber, {ip, port});
	}
}

void Mailbox::onUpdateConnectionInfo(QHostAddress const &ip, int port, int hullNumber)
{
	QList<Endpoint> toDelete;
	for (auto const &endpoint : mKnownRobots.values()) {
		if (endpoint == Endpoint{ip, port}) {
			toDelete << endpoint;
		}
	}

	for (auto const &endpoint : toDelete) {
		auto const keys = mKnownRobots.keys(endpoint);
		for (auto const &key : keys) {
			mKnownRobots.remove(key, endpoint);
		}
	}

	mKnownRobots.insertMulti(hullNumber, {ip, port});
}

void Mailbox::send(int hullNumber, QVariant const &message)
{
	for (auto const &endpoint : mKnownRobots.values(hullNumber)) {
		auto const connectionObject = connection(endpoint.ip, endpoint.port);
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

	mLock.lockForWrite();
	mMessagesQueue.enqueue(data);
	mLock.unlock();
}

bool Mailbox::hasMessages()
{
	mLock.lockForRead();
	bool const result = !mMessagesQueue.isEmpty();
	mLock.unlock();
	return result;
}

QString Mailbox::receive()
{
	mLock.lockForWrite();
	QByteArray const result = mMessagesQueue.dequeue();
	mLock.unlock();
	return QString(result);
}

