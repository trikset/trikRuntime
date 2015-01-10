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

#include "mailboxInterface.h"

namespace trikControl {

class MailboxServer;

/// Implementation of mailboxes system for real robot.
///
/// @todo Make mailbox renew its IP information when robots connects to a new network.
///       Workaround: reboot trikGui (or robot) after connectiong to a new network.
///
/// @todo Remove mailboxes from TrikControl, as they are not hardware-dependent and may work on a computer or in
///       2d model too.
class Mailbox : public MailboxInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - port for mailbox server.
	Mailbox(int port);

	~Mailbox() override;

	void setHullNumber(int myHullNumber) override;

	QHostAddress serverIp() const override;

	QHostAddress myIp() const override;

public slots:
	void connect(QString const &ip, int port) override;

	void connect(QString const &ip) override;

	void send(int hullNumber, QString const &message) override;

	void send(QString const &message) override;

	bool hasMessages() override;

	QString receive() override;

	int myHullNumber() const override;

signals:
	/// Used to interrupt waiting for new message.
	void stopWaiting();

private:
	/// Server that works in separate thread.
	QScopedPointer<MailboxServer> mWorker;

	/// Worker thread.
	QThread mWorkerThread;
};

}
