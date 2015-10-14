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
///
/// @todo Make mailbox renew its IP information when robots connects to a new network.
///       Workaround: reboot trikGui (or robot) after connectiong to a new network.
class Mailbox : public MailboxInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - port for mailbox server.
	Mailbox(int port);

	/// Constructor.
	/// @param configurer - configurer object that contains preparsed XML config.
	Mailbox(const trikKernel::Configurer &configurer);

	~Mailbox() override;

	bool isConnected() const override;

	void setHullNumber(int myHullNumber) override;

	QHostAddress serverIp() const override;

	QHostAddress myIp() const override;

	void clearQueue() override;

	bool isEnabled() override;

	void stopWaiting() override;

public slots:
	void connect(const QString &ip, int port) override;

	void connect(const QString &ip) override;

	void send(int hullNumber, const QString &message) override;

	void send(const QString &message) override;

	bool hasMessages() override;

	QString receive(bool wait = true) override;

	int myHullNumber() const override;

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
