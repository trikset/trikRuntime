/* Copyright 2013 Yurii Litvinov
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

namespace trikScriptRunner {
class TrikScriptRunner;
}

namespace trikCommunicator {

class ScriptRunnerWrapper;
class Connection;

/// Class that enables connection with a client running on computer (TrikLab or remote control).
/// Communication subsystem consists of TrikCommunicator object which runs in main thread and listens for incoming
/// connections, Connection objects --- one for every connected client, they run in separate threads each, and
/// ScriptRunnerWrapper object in main thread which processes signals from Connections.
///
/// Communicator accepts commands:
/// - file:<file name>:<file contents> --- save given contents to a file with given name in current directory.
/// - run:<file name> --- execute a file with given name.
/// - stop --- stop current script execution and a robot.
/// - direct:<command> --- execute given script without saving it to a file.
/// - keepalive --- do nothing, used to check the availability of connection.
class TrikCommunicator : public QTcpServer
{
	Q_OBJECT

public:
	/// Constructor that creates its own instance of a script runner.
	/// @param configPath - path to config file for trikControl, for example, /home/root/trik/.
	explicit TrikCommunicator(QString const &configFilePath);

	/// Constructor that accepts external script runner and issues commands to it.
	explicit TrikCommunicator(trikScriptRunner::TrikScriptRunner &runner);

	~TrikCommunicator();

	/// Starts listening given port on all network interfaces.
	void startServer(int const &port);

protected:
	void incomingConnection(int socketDescriptor);  // Override.

private slots:
	/// Called when connection thread finishes.
	void onConnectionClosed();

private:
	/// Script runner object common to all connections.
	QScopedPointer<ScriptRunnerWrapper> mScriptRunnerWrapper;

	/// Maps thread object to corresponding connection worker object, to be able to correctly stop and delete them all.
	QHash<QThread *, Connection *> mConnections;  // Has ownership on threads and connections.
};

}
