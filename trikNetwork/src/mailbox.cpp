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
	return mWorker->isConnected();
}

void Mailbox::setHullNumber(int hullNumber)
{
	if (isEnabled()) {
		QMetaObject::invokeMethod(mWorker.data(), "setHullNumber", Q_ARG(int, hullNumber));
	}
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

void Mailbox::clearQueue()
{
	while (mWorker->hasMessages()) {
		mWorker->receive();
	}
}

void Mailbox::stopWaiting()
{
	emit stopWaitingSignal();
}

bool Mailbox::isEnabled()
{
	return !mWorker.isNull();
}

void Mailbox::connect(const QString &ip, int port)
{
	QMetaObject::invokeMethod(mWorker.data(), "connect", Q_ARG(const QString &, ip), Q_ARG(int, port));
}

void Mailbox::connect(const QString &ip)
{
	QMetaObject::invokeMethod(mWorker.data(), "connect", Q_ARG(const QString &, ip));
}

void Mailbox::send(int hullNumber, const QString &message)
{
	QMetaObject::invokeMethod(mWorker.data(), "send", Q_ARG(int, hullNumber), Q_ARG(const QString &, message));
}

void Mailbox::send(const QString &message)
{
	QMetaObject::invokeMethod(mWorker.data(), "send", Q_ARG(const QString &, message));
}

bool Mailbox::hasMessages()
{
	return mWorker->hasMessages();
}

QString Mailbox::receive(bool wait)
{
	QString result;

	QEventLoop loop;
	QObject::connect(this, SIGNAL(stopWaitingSignal()), &loop, SLOT(quit()));
	if (!mWorker->hasMessages() && wait) {
		loop.exec();
	}

	if (mWorker->hasMessages()) {
		result = mWorker->receive();
	}

	return result;
}

void Mailbox::init(int port)
{
	mWorker.reset(new MailboxServer(port));
	QObject::connect(mWorker.data(), SIGNAL(newMessage(int, QString)), this, SIGNAL(newMessage(int, QString)));
	QObject::connect(mWorker.data(), SIGNAL(newMessage(int, QString)), this, SIGNAL(stopWaitingSignal()));
	QObject::connect(mWorker.data(), SIGNAL(connected()), this, SLOT(updateConnectionStatus()));
	QObject::connect(mWorker.data(), SIGNAL(disconnected()), this, SLOT(updateConnectionStatus()));

	mWorker->moveToThread(&mWorkerThread);
	mWorkerThread.start();

	QMetaObject::invokeMethod(mWorker.data(), "start", Qt::QueuedConnection);
}

void Mailbox::updateConnectionStatus()
{
	emit connectionStatusChanged(mWorker->activeConnections() > 0);
}
