/* Copyright 2014 Cybertech Labs Ltd.
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

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>
#include <QtCore/QMutex>
#include <QtNetwork/QHostAddress>

#include "declSpec.h"

namespace trikControl {

class MailboxServer;

/// Allows communication between robots. Each robot has its hull number (assigned in "Settings->Comm settings" in GUI),
/// and can connect to other robot (called "leader") by IP, if they are in the same subnet (it is also can be done
/// in "Settings->Comm settings", or from script by connect() method). Then robot can send messages to every robot
/// that is connected to a leader (or to a robot that is connected to a robot that is connected to a leader, and so on).
/// Recipients are identified by their hull numbers, if two or more robots in a network have the same hull number,
/// they will all receive a message.
///
/// @todo Make mailbox renew its IP information when robots connects to a new network.
///       Workaround: reboot trikGui (or robot) after connectiong to a new network.
class TRIKCONTROL_EXPORT Mailbox : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - port for mailbox server.
	Mailbox(int port);

	~Mailbox() override;

	/// Sets hull number of this robot and sends a message to update hull number to all known robots.
	void setHullNumber(int myHullNumber);

	/// Returns IP of a leader robot, or empty QHostAddress if we are not connected.
	QHostAddress serverIp() const;

	/// Returns our IP address, or empty QHostAddress if we are not connected.
	QHostAddress myIp() const;

public slots:
	/// Connects to robot by IP and port.
	void connect(QString const &ip, int port);

	/// Connects to robot by IP and uses port of local mailbox server as a port on remote robot.
	void connect(QString const &ip);

	/// Sends message to a robot with given hull number.
	void send(int hullNumber, QString const &message);

	/// Sends message to all known robots.
	void send(QString const &message);

	/// Returns true if there are incoming messages. Returns immediately.
	bool hasMessages();

	/// Receives and returns one incoming message. If there is already a message in a queue, returns immediately,
	/// otherwise blocks until a message is received. Note that if receive() and handler for newMessage() is used
	/// simultaneously, message will be delivered twice --- first for receive(), then to handler (or handlers).
	QString receive();

	/// Returns hull number of this robot.
	int myHullNumber() const;

signals:
	/// Emitted when new message is received from a robot with given hull number. Note that if receive() and
	/// handler for newMessage() is used simultaneously, message will be delivered twice --- first for receive(), then
	/// to handler (or handlers).
	void newMessage(int sender, QString const &message);

	/// Used to interrupt waiting for new message.
	void stopWaiting();

private:
	/// Server that works in separate thread.
	QScopedPointer<MailboxServer> mWorker;

	/// Worker thread.
	QThread mWorkerThread;
};

}
