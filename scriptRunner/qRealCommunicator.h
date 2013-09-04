#pragma once

#include <QtCore/QString>
#include <QtNetwork/QTcpServer>
#include <QtNetwork/QTcpSocket>

#include "runner.h"

namespace scriptRunner {

/// Class that keeps connection with QReal:Robots running on computer.
/// Accepts commands in form "file:<file name>:<file contents>" and "run:<file name>".
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
	bool mReadingDone;

	static QString readFromFile(QString const &fileName);
	static void writeToFile(QString const &fileName, QString const &contents);
};

}
