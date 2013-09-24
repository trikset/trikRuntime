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
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

namespace trikScriptRunner {
class TrikScriptRunner;
}

namespace trikCommunicator {

/// Class that keeps connection with a client running on computer (TrikLab or remote control).
/// Accepts commands:
/// - file:<file name>:<file contents>
/// - run:<file name>
/// - stop
/// - direct:<command>
/// - keepalive
class TrikCommunicator : public QObject
{
	Q_OBJECT

public:
	/// Constructor that creates its own instance of a script runner.
	TrikCommunicator();

	/// Constructor that accepts external script runner and issues commands to it. Takes ownership of a runner.
	explicit TrikCommunicator(trikScriptRunner::TrikScriptRunner &runner);

	~TrikCommunicator();

	/// Starts listening given port on all network interfaces.
	void listen(int const &port);

private slots:
	void onNewConnection();
	void onDisconnected();
	void onReadyRead();

private:
	QTcpServer mServer;
	QTcpSocket* mConnection;  // Has ownership.
	trikScriptRunner::TrikScriptRunner *mRunner;  // Has or doesn't have ownership, depending on mOwnsRunner.
	bool const mOwnsRunner;

	static QString readFromFile(QString const &fileName);
	static void writeToFile(QString const &fileName, QString const &contents);
};

}
