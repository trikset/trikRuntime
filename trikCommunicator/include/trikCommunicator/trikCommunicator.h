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
	TrikCommunicator();

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
	trikScriptRunner::TrikScriptRunner *mRunner;  // Has ownership.

	static QString readFromFile(QString const &fileName);
	static void writeToFile(QString const &fileName, QString const &contents);
};

}
