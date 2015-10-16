/* Copyright 2014 - 2015 CyberTech Labs Ltd.
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

#include <QtCore/QThread>

#include <trikKernel/version.h>
#include <QsLog.h>

#include "connection.h"

using namespace trikNetwork;

const int keepaliveTime = 3000;
const int heartbeatTime = 5000;

Connection::Connection(Protocol connectionProtocol, Heartbeat useHeartbeat)
	: mProtocol(connectionProtocol)
	, mUseHeartbeat(useHeartbeat == Heartbeat::use)
{
}

bool Connection::isConnected() const
{
	return mSocket->isOpen() && mSocket->isValid();
}

QHostAddress Connection::peerAddress() const
{
	if (!mSocket) {
		QLOG_FATAL() << "Connection::peerAddress() called on empty socket, thread:" << thread();
		Q_ASSERT(false);
	}

	return mSocket->peerAddress();
}

int Connection::peerPort() const
{
	if (!mSocket) {
		QLOG_FATAL() << "Connection::peerPort() called on empty socket, thread:" << thread();
		Q_ASSERT(false);
	}

	return mSocket->peerPort();
}

void Connection::init(const QHostAddress &ip, int port)
{
	mSocket.reset(new QTcpSocket());

	connectSlots();

	mSocket->connectToHost(ip, port);

	initKeepalive();

	if (!mSocket->waitForConnected()) {
		QLOG_ERROR() << "Connection to" << ip << ":" << port << "failed";
		doDisconnect();
	} else {
		if (mUseHeartbeat) {
			mKeepAliveTimer->start();
			mHeartbeatTimer->start();
		}

		emit connected(this);
	}
}

void Connection::send(const QByteArray &data)
{
	if (mSocket->state() != QAbstractSocket::ConnectedState) {
		QLOG_ERROR() << "Trying to send through unconnected socket, message is not delivered";
		return;
	}

	QLOG_INFO() << "Sending:" << data << " to" << peerAddress() << ":" << peerPort();

	if (mUseHeartbeat) {
		/// Reset keepalive timer to avoid spamming with keepalive packets.
		mKeepAliveTimer->start();
	}

	const QByteArray message = mProtocol == Protocol::messageLength
			? QByteArray::number(data.size()) + ':' + data
			: data + '\n';

	const qint64 sentBytes = mSocket->write(message);
	if (sentBytes != message.size()) {
		QLOG_ERROR() << "Failed to send message" << message << ", " << sentBytes << "sent.";
	}
}

void Connection::init(int socketDescriptor)
{
	mSocket.reset(new QTcpSocket());

	initKeepalive();

	if (!mSocket->setSocketDescriptor(socketDescriptor)) {
		QLOG_ERROR() << "Failed to set socket descriptor" << socketDescriptor;
		return;
	}

	connectSlots();

	if (mUseHeartbeat) {
		mKeepAliveTimer->start();
		mHeartbeatTimer->start();
	}
}

void Connection::onReadyRead()
{
	if (!mSocket || !mSocket->isValid()) {
		return;
	}

	if (mUseHeartbeat) {
		/// Reset heartbeat timer, we received something, so connection is up.
		mHeartbeatTimer->start();
	}

	const QByteArray &data = mSocket->readAll();
	mBuffer.append(data);

	QLOG_INFO() << "Received from" << peerAddress() << ":" << peerPort() << ":" << mBuffer;

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
				const int delimiterIndex = mBuffer.indexOf(':');
				if (delimiterIndex == -1) {
					// We did not receive full message length yet.
					return;
				} else {
					const QByteArray length = mBuffer.left(delimiterIndex);
					mBuffer = mBuffer.mid(delimiterIndex + 1);
					bool ok = false;
					mExpectedBytes = length.toInt(&ok);
					if (!ok) {
						QLOG_ERROR() << "Malformed message, can not determine message length from this:" << length;
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
			const auto messages = mBuffer.split('\n');
			for (int i = 0; i < messages.size() - 1; ++i) {
				handleIncomingData(messages.at(i));
			}

			mBuffer = messages.last();
		}
	}
	}
}

void Connection::handleIncomingData(const QByteArray &data)
{
	if (data == "keepalive") {
		return;
	} if (data == "version") {
		send(QString("version: " + trikKernel::version).toUtf8());
	} else {
		processData(data);
	}
}

void Connection::onConnect()
{
	if (mUseHeartbeat) {
		mKeepAliveTimer->start();
		mHeartbeatTimer->start();
	}
}

void Connection::onDisconnect()
{
	QLOG_INFO() << "Connection" << mSocket->socketDescriptor() << "disconnected.";

	doDisconnect();
}

void Connection::onError(QAbstractSocket::SocketError error)
{
	if (error == QAbstractSocket::RemoteHostClosedError) {
		QLOG_ERROR() << "Connection" << mSocket->socketDescriptor() << ": remote host closed";
	} else {
		QLOG_ERROR() << "Connection" << mSocket->socketDescriptor() << "errored.";
	}

	doDisconnect();
}

void Connection::keepAlive()
{
	send("keepalive");
}

void Connection::onHeartbeatTimeout()
{
	/// We did not receive anything for some time, assuming connection is down and closing socket.
	mSocket->disconnectFromHost();
}

void Connection::connectSlots()
{
	connect(mSocket.data(), SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	connect(mSocket.data(), SIGNAL(connected()), this, SLOT(onConnect()));
	connect(mSocket.data(), SIGNAL(disconnected()), this, SLOT(onDisconnect()));
	connect(mSocket.data(), SIGNAL(error(QAbstractSocket::SocketError))
			, this, SLOT(onError(QAbstractSocket::SocketError)));
}

void Connection::doDisconnect()
{
	if (mDisconnectReported) {
		return;
	}

	if (mUseHeartbeat) {
		mKeepAliveTimer->stop();
		mHeartbeatTimer->stop();
	}

	mDisconnectReported = true;

	emit disconnected(this);

	thread()->quit();
}

void Connection::initKeepalive()
{
	if (mUseHeartbeat) {
		mKeepAliveTimer.reset(new QTimer);
		mHeartbeatTimer.reset(new QTimer);

		connect(mKeepAliveTimer.data(), SIGNAL(timeout()), this, SLOT(keepAlive()));
		connect(mHeartbeatTimer.data(), SIGNAL(timeout()), this, SLOT(onHeartbeatTimeout()));

		mKeepAliveTimer->setSingleShot(false);
		mHeartbeatTimer->setSingleShot(false);

		mKeepAliveTimer->setInterval(keepaliveTime);
		mHeartbeatTimer->setInterval(heartbeatTime);
	}
}
