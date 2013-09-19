#pragma once

#include <QtCore/QString>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include "runner.h"

namespace trikScriptRunner {

/// Class that keeps connection with a client running on computer (TrikLab or remote control).
/// Accepts commands:
/// - file:<file name>:<file contents>
/// - run:<file name>
/// - stop
/// - direct:<command>
/// - keepalive
class QRealCommunicator : public QObject
{
	Q_OBJECT

public:
	QRealCommunicator();

	~QRealCommunicator();

	/// Starts listening given port on all network interfaces.
	void listen(int const &port);

private slots:
	void onNewConnection();
	void onDisconnected();
	void onReadyRead();

private:
	class SleeperThread : public QThread
	{
	public:
		static void msleep(unsigned long msecs)
		{
			QThread::msleep(msecs);
		}
	};

	QTcpServer mServer;
	QTcpSocket* mConnection;  // Has ownership.
	Runner mRunner;

	static QString readFromFile(QString const &fileName);
	static void writeToFile(QString const &fileName, QString const &contents);
};

}
