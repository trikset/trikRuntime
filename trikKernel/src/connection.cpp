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

#include "QsLog.h"

using namespace trikKernel;

Connection::Connection(Protocol connectionProtocol)
	: mProtocol(connectionProtocol)
{
}

QHostAddress Connection::peerAddress() const
{
	if (!mSocket) {
		QLOG_FATAL() << "Connection::peerAddress() called on empty socket, thread:" << thread();
		qDebug() << "Connection::peerAddress() called on empty socket, thread:" << thread();
		Q_ASSERT(false);
	}

	return mSocket->peerAddress();
}

int Connection::peerPort() const
{
	if (!mSocket) {
		QLOG_FATAL() << "Connection::peerPort() called on empty socket, thread:" << thread();
		qDebug() << "Connection::peerPort() called on empty socket, thread:" << thread();
		Q_ASSERT(false);
	}

	return mSocket->peerPort();
}

void Connection::init(QHostAddress const &ip, int port)
{
	mSocket.reset(new QTcpSocket());

	connectSlots();

	mSocket->connectToHost(ip, port);

	if (!mSocket->waitForConnected()) {
		QLOG_ERROR() << "Connection to" << ip << ":" << port << "failed";
		qDebug() << "Connection to" << ip << ":" << port << "failed";
		thread()->quit();
	}
}

void Connection::send(QByteArray const &data)
{
	if (mSocket->state() != QAbstractSocket::ConnectedState) {
		QLOG_ERROR() << "Trying to send through unconnected socket, message is not delivered";
		qDebug() << "Trying to send through unconnected socket, message is not delivered";
		return;
	}

	QLOG_INFO() << "Sending:" << data << " to" << peerAddress() << ":" << peerPort();
	qDebug() << "Sending:" << data << " to" << peerAddress() << ":" << peerPort();

	QByteArray const message = mProtocol == Protocol::messageLength
			? QByteArray::number(data.size()) + ':' + data
			: data + '\n';

	qint64 const sentBytes = mSocket->write(message);
	if (sentBytes != message.size()) {
		QLOG_ERROR() << "Failed to send message" << message << ", " << sentBytes << "sent.";
		qDebug() << "Failed to send message" << message << ", " << sentBytes << "sent.";
	}
}

void Connection::init(int socketDescriptor)
{
	mSocket.reset(new QTcpSocket());

	if (!mSocket->setSocketDescriptor(socketDescriptor)) {
		QLOG_ERROR() << "Failed to set socket descriptor" << socketDescriptor;
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
	mBuffer.append(data);

	QLOG_INFO() << "Received from" << peerAddress() << ":" << peerPort() << ":" << mBuffer;
	qDebug() << "Received from" << peerAddress() << ":" << peerPort() << ":" << mBuffer;

	processBuffer();
}

void Connection::processBuffer()
{
	switch (mProtocol) {
	case Protocol::messageLength:
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
	case Protocol::endOfLineSeparator:
	{
		if (mBuffer.contains('\n')) {
			auto const messages = mBuffer.split('\n');
			for (int i = 0; i < messages.size() - 1; ++i) {
				processData(messages.at(i));
			}

			mBuffer = messages.last();
		}
	}
	}
}

void Connection::onDisconnect()
{
	QLOG_INFO() << "Connection" << mSocket->socketDescriptor() << "disconnected.";
	qDebug() << "Connection" << mSocket->socketDescriptor() << "disconnected.";

	thread()->quit();
}

void Connection::onError(QAbstractSocket::SocketError error)
{
	Q_UNUSED(error)

	if (error == QAbstractSocket::RemoteHostClosedError) {
		QLOG_ERROR() << "Connection" << mSocket->socketDescriptor() << ": remote host closed";
		qDebug() << "Connection" << mSocket->socketDescriptor() << ": remote host closed";
	} else {
		QLOG_ERROR() << "Connection" << mSocket->socketDescriptor() << "errored.";
		qDebug() << "Connection" << mSocket->socketDescriptor() << "errored.";
	}

	thread()->quit();
}

void Connection::connectSlots()
{
	connect(mSocket.data(), SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	connect(mSocket.data(), SIGNAL(disconnected()), this, SLOT(onDisconnect()));
	connect(mSocket.data(), SIGNAL(error(QAbstractSocket::SocketError))
			, this, SLOT(onError(QAbstractSocket::SocketError)));
}
