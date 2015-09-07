/* Copyright 2013 - 2015 Yurii Litvinov, CyberTech Labs Ltd.
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

#include <QtCore/QString>
#include <QtCore/QHash>
#include <QtCore/QScopedPointer>
#include <QtCore/QThread>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include <trikNetwork/trikServer.h>

namespace trikScriptRunner {
class TrikScriptRunner;
}

namespace trikControl {
class BrickInterface;
}

namespace trikNetwork {
class MailboxInterface;
class GamepadInterface;
}

namespace trikCommunicator {

class Connection;

/// Class that enables connection with a client running on computer (TRIKStudio or remote control).
/// Communication subsystem consists of TrikCommunicator object which runs in main thread and listens for incoming
/// connections, Connection objects --- one for every connected client, they run in separate threads each, and
/// ScriptRunnerWrapper object in main thread which processes signals from Connections.
class TrikCommunicator : public trikNetwork::TrikServer
{
	Q_OBJECT

public:
	/// Constructor that creates its own instance of a script runner.
	/// @param brick - reference to robot brick.
	/// @param mailbox - mailbox object used to communicate with other robots.
	/// @param gamepad - gamepad object used to interact with TRIK Gamepad on Android device.
	/// @param startDirPath - path to the directory from which the application was executed.
	TrikCommunicator(trikControl::BrickInterface &brick
			, trikNetwork::MailboxInterface * const mailbox
			, trikNetwork::GamepadInterface * const gamepad
			);

	/// Constructor that accepts external script runner and issues commands to it.
	explicit TrikCommunicator(trikScriptRunner::TrikScriptRunner &runner);

	~TrikCommunicator() override;

public slots:
	/// Broadcasts given text as a message to be printed.
	void sendPrintMessage(const QString &text);

signals:
	/// Emitted when stop command received from a computer.
	void stopCommandReceived();

private:
	TrikCommunicator(trikScriptRunner::TrikScriptRunner * const runner, bool hasScriptRunnerOwnership);

	Connection *connectionFactory();

	/// Script runner object common to all connections.
	/// Ownership depends on mHasScriptRunnerOwnership flag, if we received runner belonging to other object or created
	/// our own.
	trikScriptRunner::TrikScriptRunner *mTrikScriptRunner;

	/// True, if we created our own script runner, false if we got it from someone.
	const bool mHasScriptRunnerOwnership;
};

}
