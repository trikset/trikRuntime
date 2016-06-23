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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QTimer>
#include <QtNetwork/QTcpSocket>
#include <QtNetwork/QHostAddress>

#include "declSpec.h"

namespace trikNetwork {

/// Connection protocol variants.
enum class Protocol
{
	/// Message is in form "<data length in bytes>:<data>".
	messageLength

	/// Message is in form "<data>\n".
	, endOfLineSeparator
};

/// Heartbeat protocol option.
enum class Heartbeat
{
	/// Wait for a packet every N milliseconds, if none is received, assume connection lost and close socket.
	use

	/// Do not use heartbeat, keep socket open until TCP protocol detects disconnect (which may take a while,
	/// so we will not be able to detect hardware network failures).
	, dontUse
};

/// Abstract class that serves one client of TrikServer. Meant to work in separate thread. Creates its own socket and
/// handles all incoming messages.
class TRIKNETWORK_EXPORT Connection : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param connectionProtocol - protocol used by this connection.
	/// @param useHeartbeat - use or don't use heartbeat protocol option.
	explicit Connection(Protocol connectionProtocol, Heartbeat useHeartbeat);

	/// Returns true if socket is opened or false otherwise.
	bool isConnected() const;

	/// Returns peer address of a connection, if it is open, or empty QHostAddress if connection is not established yet.
	QHostAddress peerAddress() const;

	/// Returns peer port of a connection, if it is open, or -1 if connection is not established yet.
	int peerPort() const;

	/// Creates socket and initializes incoming connection, shall be called when Connection is already in its own
	/// thread.
	/// @param socketDescriptor - native socket descriptor.
	Q_INVOKABLE void init(int socketDescriptor);

	/// Sends given byte array to peer.
	Q_INVOKABLE void send(const QByteArray &data);

signals:
	/// Emitted after connection becomes closed.
	void disconnected(Connection *self);

	/// Emitted after connection is established.
	void connected(Connection *self);

protected:
	/// Creates socket and initializes outgoing connection, shall be called when Connection is already in its own
	/// thread.
	/// @param ip - target ip address.
	/// @param port - target port.
	void init(const QHostAddress &ip, int port);

private slots:
	/// New data is ready on a socket.
	void onReadyRead();

	/// Socket is opened.
	void onConnect();

	/// Socket is closed for some reason.
	void onDisconnect();

	/// Socket is failed to connect or some other error occured.
	void onError(QAbstractSocket::SocketError error);

	/// Sends "keepalive" packet.
	void keepAlive();

	/// Heartbeat timer timed out, close connection.
	void onHeartbeatTimeout();

private:
	/// Processes received data. Shall be implemented in concrete connection classes.
	virtual void processData(const QByteArray &data) = 0;

	/// Handles incoming data: sending version or processing received data.
	void handleIncomingData(const QByteArray &data);

	/// Connects all slots of this object to the appropriate signals.
	void connectSlots();

	/// Parses current buffer content and splits it on complete messages.
	void processBuffer();

	/// Helper method that notifies everyone about disconnect and closes connection gracefully.
	void doDisconnect();

	/// Initializes keepalive and heartbeat timers.
	void initKeepalive();

	/// Buffer to accumulate parts of a message.
	QByteArray mBuffer;

	/// Declared size of a current message.
	int mExpectedBytes = 0;

	/// Protocol selected for this connection.
	Protocol mProtocol;

	/// Timer that is used to send keepalive packets.
	QScopedPointer<QTimer> mKeepAliveTimer;

	/// Timer that is used to check that keepalive packets from other end of the line were properly received.
	QScopedPointer<QTimer> mHeartbeatTimer;

	/// Flag that ensures that "disconnected" signal will be sent only once.
	bool mDisconnectReported = false;

	/// Use or don;t use heartbeat method of connection loss detection.
	bool mUseHeartbeat = false;

	/// Socket for this connection.
	QScopedPointer<QTcpSocket> mSocket;
};

}
