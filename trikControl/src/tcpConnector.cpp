#include "src/tcpConnector.h"

#include <QtCore/QDataStream>

using namespace trikControl;

TcpConnector::TcpConnector(int port)
	: mPort(port)
	, mTcpServer(new QTcpServer())
{
}

void TcpConnector::startServer()
{
	if (!mTcpServer->listen(QHostAddress::Any, mPort)) {
		qDebug() << "Unable to start the server:" << mTcpServer->errorString();
		mTcpServer->close();
		return;
	}

	qDebug() << "TcpServer started";
	connect(mTcpServer.data(), SIGNAL(newConnection()), this, SLOT(connection()));
}

void TcpConnector::connection()
{
	mTcpSocket.reset(mTcpServer->nextPendingConnection());
	mTcpSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
	qDebug() << "Set new connection";
	connect(mTcpSocket.data(), SIGNAL(disconnected()), this, SLOT(tcpDisconnected()));
	connect(mTcpSocket.data(), SIGNAL(readyRead()), this, SLOT(networkRead()));
}

void TcpConnector::tcpDisconnected()
{
	mTcpSocket->abort();
}

void TcpConnector::networkRead()
{
	if (!mTcpSocket->isValid()) {
		return;
	}

	QString line;
	while (mTcpSocket->bytesAvailable() > 0) {
		char data[100];
		mTcpSocket->readLine(data, 100);
		line += data;
	}

	emit dataReady(line);
}
