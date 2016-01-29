/* Copyright 2016 CyberTech Labs Ltd.
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

#include "tcpClientSimulator.h"

#include <QtNetwork/QTcpSocket>

using namespace tests::utils;

TcpClientSimulator::TcpClientSimulator(const QString &ip, int port)
	: mIp(ip)
	, mPort(port)
{
	mSocket.reset(new QTcpSocket());
	connect(mSocket.data(), SIGNAL(readyRead()), this, SLOT(onReadyRead()));
	mSocket->connectToHost(ip, port);
}

TcpClientSimulator::~TcpClientSimulator()
{
	mSocket->close();
	if (mSocket->isOpen()) {
		mSocket->waitForDisconnected();
	}
}

void TcpClientSimulator::send(const QString &data)
{
	mSocket->write(data.toUtf8());
	mSocket->waitForBytesWritten(2000);
}

QString TcpClientSimulator::latestResponse() const
{
	return mLatestResponse;
}

void TcpClientSimulator::onReadyRead()
{
	const QString data = QString::fromUtf8(mSocket->readAll());
	if (data != "keepalive") {
		mLatestResponse = data;
	}

	emit newData(data);
}
