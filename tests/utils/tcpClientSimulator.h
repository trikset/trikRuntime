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

#pragma once

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>

class QTcpSocket;

namespace tests {
namespace utils {

/// Utility class that can simulate TCP network activity.
class TcpClientSimulator : QObject
{
	Q_OBJECT

public:
	TcpClientSimulator(const QString &ip, int port);

	~TcpClientSimulator() override;

	void send(const QString &data);

	QString latestResponse() const;

signals:
	void newData(const QString &data);

private slots:
	void onReadyRead();

private:
	QScopedPointer<QTcpSocket> mSocket;

	QString mLatestResponse;

	const QString mIp;
	const int mPort;
};

}
}
