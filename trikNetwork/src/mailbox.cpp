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

#include "mailbox.h"
#include "mailboxServer.h"

#include <QtCore/QEventLoop>

#include <trikKernel/configurer.h>
#include <trikKernel/exceptions/malformedConfigException.h>

using namespace trikNetwork;

Mailbox::Mailbox(int port)
{
	init(port);
}

Mailbox::Mailbox(const trikKernel::Configurer &configurer)
{
	bool ok = false;
	const int port = configurer.attributeByDevice("mailbox", "port").toInt(&ok);
	if (!ok) {
		throw trikKernel::MalformedConfigException("Incorrect mailbox port");
	}

	init(port);
}

Mailbox::~Mailbox()
{
	if (mWorkerThread.isRunning()) {
		mWorkerThread.quit();
		mWorkerThread.wait();
	}
}

bool Mailbox::isConnected() const
{
	bool res;
	QMetaObject::invokeMethod(mWorker.data(), [this, &res](){res = mWorker->isConnected();}
							, Qt::BlockingQueuedConnection);
	return res;
}

void Mailbox::setHullNumber(int hullNumber)
{
	if (isEnabled()) {
		QMetaObject::invokeMethod(mWorker.data(), [this, hullNumber](){mWorker->setHullNumber(hullNumber);});
	}
}

int Mailbox::myHullNumber() const
{
	int res;
	QMetaObject::invokeMethod(mWorker.data(), [this, &res](){res = mWorker->hullNumber();}
							, Qt::BlockingQueuedConnection);
	return res;
}

bool Mailbox::hasServer() const
{
	bool res;
	QMetaObject::invokeMethod(mWorker.data(), [this, &res](){res = mWorker->hasServer();}
							, Qt::BlockingQueuedConnection);
	return res;
}

void Mailbox::renewIp()
{
	QMetaObject::invokeMethod(mWorker.data(), &MailboxServer::renewIp);
}

QString Mailbox::serverIp() const
{
	QHostAddress res;
	QMetaObject::invokeMethod(mWorker.data(), [this, &res](){res = mWorker->serverIp();}
							, Qt::BlockingQueuedConnection);
	return res.toString();
}

QString Mailbox::myIp() const
{
	QHostAddress res;
	QMetaObject::invokeMethod(mWorker.data(), [this, &res](){res = mWorker->myIp();}, Qt::BlockingQueuedConnection);
	return res.toString();
}

void Mailbox::clearQueue()
{
	while (!receive(false).isNull()){
		/// If no messages in queue, receive returns just QString()
	}
}

void Mailbox::stopWaiting()
{
	emit stopWaitingSignal();
}

bool Mailbox::isEnabled()
{
	bool res;
	QMetaObject::invokeMethod(mWorker.data(), [this, &res](){res = !mWorker.isNull();}
							, Qt::BlockingQueuedConnection);
	return res;
}

void Mailbox::connect(const QString &ip, int port)
{
	QMetaObject::invokeMethod(mWorker.data(), [=](){mWorker->connectTo(ip, port);});
}

void Mailbox::connect(const QString &ip)
{
	QMetaObject::invokeMethod(mWorker.data(), [=](){mWorker->connectTo(ip);});
}

void Mailbox::send(int hullNumber, const QString &message)
{
	QMetaObject::invokeMethod(mWorker.data(), [=](){mWorker->send(hullNumber, message);});
}

void Mailbox::send(const QString &message)
{
	QMetaObject::invokeMethod(mWorker.data(), [=](){mWorker->send(message);});
}

bool Mailbox::hasMessages()
{
	bool res;
	QMetaObject::invokeMethod(mWorker.data(), [this, &res](){res = mWorker->hasMessages();}
							, Qt::BlockingQueuedConnection);
	return res;
}

QString Mailbox::receive(bool wait)
{
	QString result;

	if (wait && !hasMessages()) {
		QEventLoop loop;
		QObject::connect(this, &Mailbox::stopWaitingSignal, &loop, &QEventLoop::quit, Qt::QueuedConnection);
		loop.exec();
	}

	if (hasMessages()) {
		QMetaObject::invokeMethod(mWorker.data(), [this, &result](){result = mWorker->receive();}
							, Qt::BlockingQueuedConnection);
	}

	return result;
}

void Mailbox::init(int port)
{
	mWorker.reset(new MailboxServer(port));
	mWorker->moveToThread(&mWorkerThread);
	QObject::connect(mWorker.data(), &MailboxServer::newMessage, this, &Mailbox::newMessage);
	QObject::connect(mWorker.data(), &MailboxServer::newMessage, this, &Mailbox::stopWaitingSignal);
	QObject::connect(mWorker.data(), &MailboxServer::connected, this, &Mailbox::updateConnectionStatus);
	QObject::connect(mWorker.data(), &MailboxServer::disconnected, this, &Mailbox::updateConnectionStatus);

	QLOG_INFO() << "Starting Mailbox worker thread" << &mWorkerThread;

	mWorkerThread.setObjectName(mWorker->metaObject()->className());
	mWorkerThread.start();

	QMetaObject::invokeMethod(mWorker.data(), &MailboxServer::start, Qt::QueuedConnection);
}

void Mailbox::updateConnectionStatus()
{
	int activeConnections;
	QMetaObject::invokeMethod(mWorker.data(), [this, &activeConnections](){
							activeConnections = mWorker->activeConnections();}, Qt::BlockingQueuedConnection);
	emit connectionStatusChanged(activeConnections > 0);
}
