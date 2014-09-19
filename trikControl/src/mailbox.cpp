#include "include/trikControl/mailbox.h"

#include "src/mailboxServer.h"

using namespace trikControl;

Mailbox::Mailbox(int port)
	: mWorker(new MailboxServer(port, mReceiveWaitCondition))
{
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

void Mailbox::connect(QString const &ip, int port)
{
	QMetaObject::invokeMethod(mWorker.data(), "connect", Q_ARG(QString const &, ip), Q_ARG(int, port));
}

void Mailbox::send(int hullNumber, QVariant const &message)
{
	qDebug() << "Mailbox::send";
	QMetaObject::invokeMethod(mWorker.data(), "send", Q_ARG(int, hullNumber), Q_ARG(QVariant const &, message));
}

bool Mailbox::hasMessages()
{
	return mWorker->hasMessages();
}

QString Mailbox::receive()
{
	qDebug() << "Mailbox::receive()";
	QString result;
	mReceiveMutex.lock();
	if (!mWorker->hasMessages()) {
		mReceiveWaitCondition.wait(&mReceiveMutex);
	}

	result = mWorker->receive();
	mReceiveMutex.unlock();
	return result;
}
