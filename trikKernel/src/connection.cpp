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

#include <QtCore/QDebug>
#include <QtCore/QThread>

#include "connection.h"

using namespace trikKernel;

QHostAddress Connection::peerAddress() const
{
	return mSocket->peerAddress();
}

int Connection::peerPort() const
{
	return mSocket->peerPort();
}

void Connection::onInit(QHostAddress const &ip, int port)
{
	qDebug() << "Connection::onInit(" << ip << ", " << port << ")";

	mSocket.reset(new QTcpSocket());
	connectSlots();
	mSocket->connectToHost(ip, port);
	mSocket->waitForConnected();
}

void Connection::onSend(QByteArray const &data)
{
	qDebug() << "Sending to" << peerAddress() << ":" << peerPort() << ":" << data;

	mSocket->write(data);
	mSocket->flush();
	mSocket->waitForBytesWritten();
}

void Connection::onInit(int socketDescriptor)
{
	qDebug() << "Connection::onInit(" << socketDescriptor << ")";

	mSocket.reset(new QTcpSocket());

	if (!mSocket->setSocketDescriptor(socketDescriptor)) {
		qDebug() << "Failed to set socket descriptor" << socketDescriptor;
		return;
	}

	connectSlots();
}

void Connection::onReadyRead()
{
	if (!mSocket || !mSocket->isValid()) {
		return;
	}

	QByteArray const &data = mSocket->readAll();
	QString const stringData = QString::fromUtf8(data.constData());

	qDebug() << "Received from" << peerAddress() << ":" << peerPort() << ":" << stringData;

	processData(stringData);
}

void Connection::disconnected()
{
	qDebug() << "Connection" << mSocket->socketDescriptor() << "disconnected.";

	thread()->quit();
}

void Connection::errored(QAbstractSocket::SocketError error)
{
	Q_UNUSED(error)

	qDebug() << "Connection" << mSocket->socketDescriptor() << "errored.";

	thread()->quit();
}

void Connection::connectSlots()
{
	connect(mSocket.data(), SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	connect(mSocket.data(), SIGNAL(disconnected()), this, SLOT(disconnected()));
	connect(mSocket.data(), SIGNAL(error(QAbstractSocket::SocketError))
			, this, SLOT(errored(QAbstractSocket::SocketError)));
}
