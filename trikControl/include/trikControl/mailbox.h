/* Copyright 2014 Cybertech Labs Ltd.
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
#include <QtCore/QMultiHash>
#include <QtNetwork/QHostAddress>

#include <trikKernel/trikServer.h>

#include "declSpec.h"

namespace trikControl {

class TRIKCONTROL_EXPORT Mailbox : public trikKernel::TrikServer
{
	Q_OBJECT

public:
	Mailbox(QString const &ip, int port, int hullNumber);
	void setHullNumber(int hullNumber);

	void connect(QString const &ip, int port);

public slots:
//	void send(int hullNumber, QVariant const &message);
//	bool hasMessages();
//	QString receive();

private slots:
	void onNewConnection(QHostAddress const &ip, int port);

private:
	trikKernel::Connection *connectionFactory();

	int mHullNumber;
	QHostAddress const mIp;
	int const mPort;

	struct Endpoint {
		QHostAddress hostAddress;
		int port;
	};

	QMultiHash<int, Endpoint> mKnownRobots;
};

}
