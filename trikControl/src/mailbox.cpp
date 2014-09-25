#include "include/trikControl/mailbox.h"

#include "src/mailboxServer.h"

using namespace trikControl;

Mailbox::Mailbox(int port)
	: mWorker(new MailboxServer(port, mReceiveWaitCondition))
{
	QObject::connect(mWorker.data(), SIGNAL(newMessage(int, QString)), this, SIGNAL(newMessage(int, QString)));

	mWorker->moveToThread(&mWorkerThread);
	mWorkerThread.start();
}

Mailbox::~Mailbox()
{
	mWorkerThread.quit();
	mWorkerThread.wait();
}

void Mailbox::setHullNumber(int hullNumber)
{
	QMetaObject::invokeMethod(mWorker.data(), "setHullNumber", Q_ARG(int, hullNumber));
}

int Mailbox::myHullNumber() const
{
	return mWorker->hullNumber();
}

QHostAddress Mailbox::serverIp() const
{
	return mWorker->serverIp();
}

QHostAddress Mailbox::myIp() const
{
	return mWorker->myIp();
}

void Mailbox::connect(QString const &ip, int port)
{
	QMetaObject::invokeMethod(mWorker.data(), "connect", Q_ARG(QString const &, ip), Q_ARG(int, port));
}

void Mailbox::connect(QString const &ip)
{
	QMetaObject::invokeMethod(mWorker.data(), "connect", Q_ARG(QString const &, ip));
}

void Mailbox::send(int hullNumber, QString const &message)
{
	QMetaObject::invokeMethod(mWorker.data(), "send", Q_ARG(int, hullNumber), Q_ARG(QString const &, message));
}

void Mailbox::send(QString const &message)
{
	QMetaObject::invokeMethod(mWorker.data(), "send", Q_ARG(QString const &, message));
}

bool Mailbox::hasMessages()
{
	return mWorker->hasMessages();
}

QString Mailbox::receive()
{
	QString result;

	/// @todo Do not block thread entirely. It will make impossible to interrupt a program from QReal.
	mReceiveMutex.lock();
	if (!mWorker->hasMessages()) {
		mReceiveWaitCondition.wait(&mReceiveMutex);
	}

	result = mWorker->receive();
	mReceiveMutex.unlock();
	return result;
}
