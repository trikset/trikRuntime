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

#pragma once

#include <QtCore/QScopedPointer>
#include <QtCore/QThread>
#include <QtXml/QDomElement>

#include "mailboxInterface.h"

namespace trikKernel {
class Configurer;
}

namespace trikNetwork {

class MailboxServer;

/// Implementation of mailboxes system.
class Mailbox : public MailboxInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - port for mailbox server.
	explicit Mailbox(int port);

	/// Constructor.
	/// @param configurer - configurer object that contains preparsed XML config.
	explicit Mailbox(const trikKernel::Configurer &configurer);

	~Mailbox() override;

	Q_INVOKABLE bool isConnected() const override;

	Q_INVOKABLE QString serverIp() const override;

	Q_INVOKABLE QString myIp() const override;

	Q_INVOKABLE bool isEnabled() override;

	Q_INVOKABLE bool hasMessages() override;

	Q_INVOKABLE QString receive(bool wait = true) override;

	Q_INVOKABLE int myHullNumber() const override;

	/// True iff the server is running.
	Q_INVOKABLE bool hasServer() const;

public slots:
	void connect(const QString &ip, int port) override;

	void connect(const QString &ip) override;

	void send(int hullNumber, const QString &message) override;

	void send(const QString &message) override;

	void stopWaiting() override;

	void renewIp() override;

	void clearQueue() override;

	void setHullNumber(int myHullNumber) override;

signals:
	/// Used to interrupt waiting for new message.
	void stopWaitingSignal();

private slots:
	void updateConnectionStatus();

private:
	/// Starts mailbox listening given port.
	void init(int port);

	/// Server that works in separate thread.
	QScopedPointer<MailboxServer> mWorker;

	/// Worker thread.
	QThread mWorkerThread;
};

}
