/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
