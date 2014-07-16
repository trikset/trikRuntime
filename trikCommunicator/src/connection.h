/* Copyright 2014 CyberTech Labs Ltd.
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
#include <QtNetwork/QTcpSocket>

namespace trikScriptRunner {
class TrikScriptRunner;
}

namespace trikCommunicator {

/// Class that serves one client of TrikCommunicator. Meant to work in separate thread. Creates its own socket and
/// handles all incoming messages, calling ScriptRunnerWrapper for brick functionality.
///
/// Connection accepts commands:
/// - file:<file name>:<file contents> --- save given contents to a file with given name in current directory.
/// - run:<file name> --- execute a file with given name.
/// - stop --- stop current script execution and a robot.
/// - direct:<command> --- execute given script without saving it to a file.
/// - keepalive --- do nothing, used to check the availability of connection.
class Connection : public QObject {
	Q_OBJECT

public:
	Connection();

signals:
	/// Emitted when command to run a script from a file is received.
	void startedScript(QString const &scriptFileName);

	/// Emitted when command to run script directly is received.
	void startedDirectScript();

public slots:
	/// Creates socket and initializes connection, shall be called when Connection is already in its own thread.
	/// @param socketDescriptor - native socket descriptor.
	/// @param scriptRunnerWrapper - instance of script runner object.
	void init(int socketDescriptor, trikScriptRunner::TrikScriptRunner *trikScriptRunner);

private slots:
	/// New data is ready on a socket.
	void onReadyRead();

	/// Socket is closed for some reason.
	void disconnected();

private:
	/// Socket for this connection.
	QScopedPointer<QTcpSocket> mSocket;

	/// Common script runner object, located in another thread.
	/// Does not have ownership.
	trikScriptRunner::TrikScriptRunner *mTrikScriptRunner;
};

}
