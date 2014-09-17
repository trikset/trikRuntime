#include "include/trikControl/mailbox.h"

#include "src/mailboxConnection.h"

using namespace trikControl;

Mailbox::Mailbox(QString const &ip, int port, int hullNumber)
	: trikKernel::TrikServer([this] () { return connectionFactory(); })
	, mHullNumber(hullNumber)
	, mIp(ip)
	, mPort(port)
{
	startServer(port);
}

void Mailbox::setHullNumber(int hullNumber)
{
	mHullNumber = hullNumber;
}

void Mailbox::connect(QString const &ip, int port)
{
	auto connection = new MailboxConnection();

	startConnection(connection);

	QMetaObject::invokeMethod(connection, "connect"
			, Q_ARG(QString const &, mIp)
			, Q_ARG(QString const &, ip)
			, Q_ARG(int, port)
			, Q_ARG(int, mHullNumber)
			);
}

trikKernel::Connection *Mailbox::connectionFactory()
{
	auto connection = new MailboxConnection();

	QObject::connect(connection, SIGNAL(newConnection(QHostAddress, int))
			, this, SLOT(onNewConnection(QHostAddress, int)));

	return connection;
}

void Mailbox::onNewConnection(QHostAddress const &ip, int port)
{
	Q_UNUSED(ip)
	Q_UNUSED(port)
}
