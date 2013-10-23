#include "src/tcpConnector.h"
#include <QDataStream>

using namespace trikControl;

TcpConnector::TcpConnector(int port):
    mPort(port),
    nextBlockSize(true)
{
}

void TcpConnector::startServer()
{
    mTcpServer = new QTcpServer();
    if (!mTcpServer->listen(QHostAddress::Any, mPort))
    {
        qDebug()<<": unable to start the server:"<<mTcpServer->errorString();
        mTcpServer->close();
        return;
    }
    qDebug() << "TcpServer starts";
    connect(mTcpServer, SIGNAL(newConnection()), this, SLOT(connection()));
}

void TcpConnector::connection()
{
    mTcpSocket = QSharedPointer<QTcpSocket>(mTcpServer->nextPendingConnection());
    mTcpSocket->setSocketOption(QAbstractSocket::LowDelayOption, 1);
    qDebug()<<": set new connection";
    connect(mTcpSocket.data(), SIGNAL(disconnected()), this, SLOT(tcpDisconnected()));
    connect(mTcpSocket.data(), SIGNAL(readyRead()), this, SLOT(networkRead()));
}

void TcpConnector::tcpDisconnected()
{
    mTcpSocket->abort();
}

void TcpConnector::networkRead()
{
    if (!mTcpSocket->isValid()) return;

    while (mTcpSocket->bytesAvailable() > 0)
    {
        char data[100];
        mTcpSocket->readLine(data, 100);
        QString command(data);
        line = command;
    }

    emit dataReady(line);
}
