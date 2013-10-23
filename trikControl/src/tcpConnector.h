#pragma once

#include <QtCore/QObject>
#include <QTcpSocket>
#include <QTcpServer>
#include <QSharedPointer>
#include "declSpec.h"

namespace trikControl {

class TRIKCONTROL_EXPORT TcpConnector : public QObject
{
    Q_OBJECT
public:
    TcpConnector(int port);

signals:
    void dataReady(QString message);

public slots:
    void startServer();

private slots:
    void tcpDisconnected();
    void connection();
    void networkRead();

private:
    int mPort;
    int nextBlockSize;
    QString line;
    QTcpServer* mTcpServer;
    QSharedPointer<QTcpSocket> mTcpSocket;
};

}
