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

Mailbox::Mailbox(trikKernel::Configurer const &configurer)
{
	bool ok = false;
	int const port = configurer.attributeByDevice("mailbox", "port").toInt(&ok);
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

void Mailbox::reset()
{
	emit stopWaiting();
	while (mWorker->hasMessages()) {
		mWorker->receive();
	}
}

bool Mailbox::isEnabled()
{
	return !mWorker.isNull();
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

	QEventLoop loop;
	QObject::connect(this, SIGNAL(stopWaiting()), &loop, SLOT(quit()));
	if (!mWorker->hasMessages()) {
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
	QObject::connect(mWorker.data(), SIGNAL(newMessage(int, QString)), this, SIGNAL(stopWaiting()));

	mWorker->moveToThread(&mWorkerThread);
	mWorkerThread.start();
}
