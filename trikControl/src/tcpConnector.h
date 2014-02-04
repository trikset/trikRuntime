#pragma once

#include <QtCore/QObject>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QTcpServer>
#include <QtCore/QScopedPointer>

#include "declSpec.h"

namespace trikControl {

/// TCP server.
class TRIKCONTROL_EXPORT TcpConnector : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - TCP port of a server.
	TcpConnector(int port);

signals:
	/// Emitted when there is incoming TCP message.
	void dataReady(QString const &message);

public slots:
	/// Starts a server and begins listening port for incoming connections.
	void startServer();

private slots:
	void tcpDisconnected();
	void connection();
	void networkRead();

private:
	int mPort;
	QScopedPointer<QTcpServer> mTcpServer;
	QScopedPointer<QTcpSocket> mTcpSocket;
};

}
