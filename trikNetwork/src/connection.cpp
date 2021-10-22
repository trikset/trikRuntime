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
	return isValid() && mSocket->isOpen();
}

bool Connection::isValid() const
{
	return mSocket && mSocket->isValid();
}

QHostAddress Connection::peerAddress() const
{
	return isValid() ? mSocket->peerAddress() : QHostAddress();
}

int Connection::peerPort() const
{
	return isValid() ? mSocket->peerPort() : -1;
}

void Connection::init(const QHostAddress &ip, int port)
{
	resetSocket();
	restartKeepalive();

	mSocket->connectToHost(ip, port);

	if (!mSocket->waitForConnected(1000)) {
		QLOG_ERROR() << "Connection to" << ip << ":" << port << "failed with " << mSocket->error();
		doDisconnect();
		return;
	}

	emit connected(this);
}

void Connection::send(const QByteArray &data)
{
	if (!isValid() || mSocket->state() != QAbstractSocket::ConnectedState) {
		QLOG_ERROR() << "Trying to send through unconnected socket, message is not delivered";
		return;
	}

	if (data != "keepalive") {
		QLOG_INFO() << "Sending:" << data << " to" << peerAddress() << ":" << peerPort();
	}

	if (mUseHeartbeat) {
		/// Reset keepalive timer to avoid spamming with keepalive packets.
		mKeepAliveTimer->start();
	}

	const QByteArray message = mProtocol == Protocol::messageLength
			? QByteArray::number(data.size()) + ':' + data
			: data;

	const qint64 sentBytes = mSocket->write(message);
	if (sentBytes != message.size()) {
		QLOG_ERROR() << "Failed to send message" << message << ", " << sentBytes << "sent.";
	}
}

void Connection::init(qintptr socketDescriptor)
{
	resetSocket();

	if (!mSocket->setSocketDescriptor(socketDescriptor)) {
		QLOG_ERROR() << "Failed to set socket descriptor" << socketDescriptor;
		return;
	}

	restartKeepalive();
}

void Connection::onReadyRead()
{
	if (mUseHeartbeat) {
		/// Reset heartbeat timer, we received something, so connection is up.
		mHeartbeatTimer->start();
	}

	const QByteArray &data = mSocket->readAll();
	mBuffer.append(data);
	if (mBuffer != "9:keepalive") {
		QLOG_INFO() << "Received from" << peerAddress() << ":" << peerPort() << ":" << mBuffer;
	}

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
		break;
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
		break;
	}
	}
}

void Connection::handleIncomingData(const QByteArray &data)
{
	if (data == "keepalive") {
		return;
	} if (data == "version") {
		send(QString("version: %1").arg(trikKernel::version).toUtf8());
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
	doDisconnect();
}

void Connection::onError(QAbstractSocket::SocketError error)
{
	QLOG_ERROR() << "Connection" << mSocket->socketDescriptor() << "errored." << error;
	doDisconnect();
}

void Connection::keepAlive()
{
	send("keepalive");
}

void Connection::onHeartbeatTimeout()
{
	if(mSocket) {
		/// We did not receive anything for some time, assuming connection is down and closing socket.
		mSocket->disconnectFromHost();
	}
}

void Connection::resetSocket()
{
	mSocket.reset(new QTcpSocket());

	mSocket->setSocketOption(QAbstractSocket::SocketOption::KeepAliveOption, 1);
	mSocket->setSocketOption(QAbstractSocket::SocketOption::LowDelayOption, 1);
	mSocket->setSocketOption(QAbstractSocket::SocketOption::TypeOfServiceOption, 64); // Immediate level

	connect(mSocket.data(), &QTcpSocket::readyRead, this, &Connection::onReadyRead);
	connect(mSocket.data(), &QTcpSocket::connected, this, &Connection::onConnect);
	connect(mSocket.data(), &QTcpSocket::disconnected, this, &Connection::onDisconnect);
	connect(mSocket.data(), QOverload<QAbstractSocket::SocketError>::of(&QTcpSocket::error)
			, this, &Connection::onError);
}

void Connection::doDisconnect()
{
	if (mDisconnectReported) {
		return;
	}

	mDisconnectReported = true;

	QLOG_INFO() << "Connection" << mSocket->socketDescriptor() << "disconnected.";

	emit disconnected(this);
}

void Connection::restartKeepalive()
{
	if (!mUseHeartbeat) {
		return;
	}

	mKeepAliveTimer.reset(new QTimer);
	mHeartbeatTimer.reset(new QTimer);

	connect(mKeepAliveTimer.data(), &QTimer::timeout, this, &Connection::keepAlive);
	connect(mHeartbeatTimer.data(), &QTimer::timeout, this, &Connection::onHeartbeatTimeout);
	connect(this, &Connection::disconnected, mKeepAliveTimer.data(), &QTimer::stop);
	connect(this, &Connection::disconnected, mHeartbeatTimer.data(), &QTimer::stop);

	mKeepAliveTimer->setSingleShot(false);
	mHeartbeatTimer->setSingleShot(false);

	mKeepAliveTimer->setInterval(keepaliveTime);
	mHeartbeatTimer->setInterval(heartbeatTime);
	mKeepAliveTimer->start();
	mHeartbeatTimer->start();
}
