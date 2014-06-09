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

namespace trikCommunicator {

class ScriptRunnerWrapper;

/// Class that serves one client of TrikCommunicator. Meant to work in separate thread. Creates its own socket and
/// handles all incoming messages, calling ScriptRunnerWrapper for brick functionality.
class Connection : public QObject {
	Q_OBJECT

public:
	Connection();

public slots:
	/// Creates socket and initializes connection, shall be called when Connection is already in its own thread.
	void init(int socketDescriptor, ScriptRunnerWrapper *scriptRunnerWrapper);

private slots:
	/// New data is ready on a socket.
	void onReadyRead();

	/// Socket is closed for some reason.
	void disconnected();

private:
	static QString readFromFile(QString const &fileName);
	static void writeToFile(QString const &fileName, QString const &contents);

	/// Socket for this connection.
	QScopedPointer<QTcpSocket> mSocket;

	/// Common script runner object, located in another thread.
	ScriptRunnerWrapper *mScriptRunnerWrapper;  // Does not have ownership.
};

}
