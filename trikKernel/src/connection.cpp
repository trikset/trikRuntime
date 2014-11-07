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
#include "version.h"

using namespace trikKernel;

Connection::Connection(Protocol connectionProtocol)
	: mProtocol(connectionProtocol)
{
}

QHostAddress Connection::peerAddress() const
{
	if (!mSocket) {
		qDebug() << "Connection::peerAddress() called on empty socket, thread:" << thread();
		Q_ASSERT(false);
	}

	return mSocket->peerAddress();
}

int Connection::peerPort() const
{
	if (!mSocket) {
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
		qDebug() << "Connection to" << ip << ":" << port << "failed";
		thread()->quit();
	}
}

void Connection::send(QByteArray const &data)
{
	if (mSocket->state() != QAbstractSocket::ConnectedState) {
		qDebug() << "Trying to send through unconnected socket, message is not delivered";
		return;
	}

	qDebug() << "Sending:" << data << " to" << peerAddress() << ":" << peerPort();

	QByteArray const message = mProtocol == Protocol::messageLength
			? QByteArray::number(data.size()) + ':' + data
			: data + '\n';

	qint64 const sentBytes = mSocket->write(message);
	if (sentBytes != message.size()) {
		qDebug() << "Failed to send message" << message << ", " << sentBytes << "sent.";
	}
}

void Connection::init(int socketDescriptor)
{
	mSocket.reset(new QTcpSocket());

	if (!mSocket->setSocketDescriptor(socketDescriptor)) {
		qDebug() << "Failed to set socket descriptor" << socketDescriptor;
		return;
	}

	connectSlots();
}

void Connection::onReadyRead()
{
	qDebug() << "Connection::onReadyRead";
	if (!mSocket || !mSocket->isValid()) {
		return;
	}

	QByteArray const &data = mSocket->readAll();
	mBuffer.append(data);

	qDebug() << "Received from" << peerAddress() << ":" << peerPort() << ":" << mBuffer;

	processBuffer();
}

void Connection::processBuffer()
{
	qDebug() << "Connection::processBuffer";
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

					handleIncomingData(message);

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
				handleIncomingData(messages.at(i));
			}

			mBuffer = messages.last();
		}
	}
	}
}

void Connection::handleIncomingData(QByteArray const &data)
{
	qDebug() << "Connection::handleIncomingData" << data;
	if (data == "version") {
		send(QString("version: " + version).toUtf8());
	} else {
		processData(data);
	}
}

void Connection::onDisconnect()
{
	qDebug() << "Connection disconnected.";

	thread()->quit();
}

void Connection::onError(QAbstractSocket::SocketError error)
{
	Q_UNUSED(error)

	if (error == QAbstractSocket::RemoteHostClosedError) {
		qDebug() << "Connection" << mSocket->socketDescriptor() << ": remote host closed";
	} else {
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
