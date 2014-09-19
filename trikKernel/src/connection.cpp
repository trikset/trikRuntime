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
	if (!mSocket) {
		qDebug() << "Connection::peerAddress() called on empty socket, thread:" << thread();
	}

	return mSocket->peerAddress();
}

int Connection::peerPort() const
{
	if (!mSocket) {
		qDebug() << "Connection::peerPort() called on empty socket, thread:" << thread();
	}

	return mSocket->peerPort();
}

void Connection::onInit(QHostAddress const &ip, int port)
{
	qDebug() << "Connection::onInit(), thread:" << thread();
	qDebug() << "Connection::onInit(" << ip << ", " << port << ")";

	qDebug() << "Opening socket...";
	mSocket.reset(new QTcpSocket());

	connectSlots();

	qDebug() << "Connecting...";

	mSocket->connectToHost(ip, port);

	mSocket->waitForConnected();

	qDebug() << "Done!";
}

void Connection::onSend(QByteArray const &data)
{
	qDebug() << "Connection::onSend(), thread:" << thread();
	qDebug() << "Sending to" << peerAddress() << ":" << peerPort() << ":" << data;

	QByteArray message = QByteArray::number(data.size()) + ':' + data;

	mSocket->write(message);

	qDebug() << "Sending done";
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
	qDebug() << "Connection::onReadyRead(), thread:" << thread();

	if (!mSocket || !mSocket->isValid()) {
		return;
	}

	QByteArray const &data = mSocket->readAll();
	mBuffer.append(data);

	qDebug() << "Connection::onReadyRead(), after mBuffer.append(data); thread:" << thread();

	qDebug() << "Received from" << peerAddress() << ":" << peerPort() << ":" << mBuffer;

	processBuffer();
}

void Connection::processBuffer()
{
	while (!mBuffer.isEmpty()) {
		if (mExpectedBytes == 0) {
			// Determining the length of a message.
			int const delimiterIndex = mBuffer.indexOf(':');
			if (delimiterIndex == -1) {
				// We did not receive full message length yet.
				return;
			} else {
				QByteArray const length = mBuffer.left(delimiterIndex);
				mBuffer = mBuffer.mid(delimiterIndex + 1);
				bool ok;
				mExpectedBytes = length.toInt(&ok);
				if (!ok) {
					qDebug() << "Malformed message, can not determine message length from this:" << length;
					mExpectedBytes = 0;
				}
			}
		} else {
			if (mBuffer.size() >= mExpectedBytes) {
				QByteArray const message = mBuffer.left(mExpectedBytes);
				mBuffer = mBuffer.mid(mExpectedBytes);

				processData(message);

				mExpectedBytes = 0;
			} else {
				// We don't have all message yet.
				return;
			}
		}
	}
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
