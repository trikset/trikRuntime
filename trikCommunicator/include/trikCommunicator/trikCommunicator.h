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
#include <QtCore/QScopedPointer>

#include <trikKernel/differentOwnerPointer.h>
#include <trikNetwork/trikServer.h>

namespace trikScriptRunner {
class TrikScriptRunner;
}

namespace trikControl {
class BrickInterface;
}

namespace trikNetwork {
class MailboxInterface;
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
	/// @param startDirPath - path to the directory from which the application was executed.
	TrikCommunicator(trikControl::BrickInterface &brick
			, trikNetwork::MailboxInterface * const mailbox
			);

	/// Constructor that accepts external script runner and issues commands to it.
	/// @param runner - script runner that shall be used to execute commands.
	/// @param configVersion - version of system config file that can be used to check that client and runtime configs
	///        match.
	explicit TrikCommunicator(trikScriptRunner::TrikScriptRunner &runner, const QString &configVersion);

	~TrikCommunicator() override;

signals:
	/// Emitted when stop command received from a computer.
	void stopCommandReceived();

private:
	TrikCommunicator(const trikKernel::DifferentOwnerPointer<trikScriptRunner::TrikScriptRunner> &runner
			, const QString &configVersion);

	Connection *connectionFactory();

	/// Script runner object common to all connections.
	trikKernel::DifferentOwnerPointer<trikScriptRunner::TrikScriptRunner> mTrikScriptRunner;

	/// Version of system configuration file. Must correspond to casing model, used to allow clients to check that
	/// their expectations of configuration match actual one.
	const QString mConfigVersion;
};

}
