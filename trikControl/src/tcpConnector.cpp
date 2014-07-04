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

#include "src/tcpConnector.h"

#include <QtCore/QDataStream>

using namespace trikControl;

TcpConnector::TcpConnector(int port)
	: mPort(port)
{
}

void TcpConnector::startServer()
{
	mTcpServer.reset(new QTcpServer());

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
