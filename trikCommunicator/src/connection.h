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
#include <trikNetwork/connection.h>

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
class Connection : public trikNetwork::Connection {
	Q_OBJECT

public:
	/// Constructor.
	/// @param trikScriptRunner - instance of script runner object.
	explicit Connection(trikScriptRunner::TrikScriptRunner &trikScriptRunner);

	/// Writes a given message to inner socket.
	void sendMessage(QString const &message);

private:
	void processData(QByteArray const &data) override;

	/// Common script runner object, located in another thread.
	trikScriptRunner::TrikScriptRunner &mTrikScriptRunner;
};

}
