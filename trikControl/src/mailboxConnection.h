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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtNetwork/QTcpSocket>
#include <trikKernel/connection.h>

namespace trikControl {

class MailboxConnection : public trikKernel::Connection {
	Q_OBJECT

public:
	MailboxConnection();

signals:
	void newConnection(QHostAddress const &ip, int port, int hullNumber);
	void updateConnectionInfo(QHostAddress const &ip, int port, int hullNumber);
	void newData(QHostAddress const &ip, int port, QByteArray const &newData);

public slots:
	void onConnect(QString const &targetIp, int targetPort, int myHullNumber);
	void onUpdateConnectionInfo(QHostAddress const &ip, int port, int hullNumber);

private:
	void processData(QString const &data) override;
};

}
