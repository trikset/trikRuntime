/* Copyright 2018 Anastasiia Kornilova, CyberTech Labs Ltd.
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
#include <QtNetwork/QTcpServer>
#include "trikScriptRunnerDeclSpec.h"

namespace trikScriptRunner {

/// Class for script variables observing, based on HTTP server
class TRIKSCRIPTRUNNER_EXPORT TrikVariablesServer : public QObject
{
	Q_OBJECT
public:
	/// Constructor
	TrikVariablesServer();

signals:
	/// Emitted when there is a new connection with a HTTP-request of variables values
	/// @param propertyName - name of variables prefix, i.e prefix "web" for variable "web.light"
	void getVariables(const QString &propertyName);

public slots:
	/// Sends HTTP response with JSON data about variables
	/// @param json - JSON container for variables values
	void sendHTTPResponse(const QJsonObject &json);

private slots:
	/// Appends new connection for handling it
	void onNewConnection();

	/// Process incoming HTTP request which was sent through current connection
	void processHTTPRequest();

private:
	QScopedPointer<QTcpServer> mTcpServer;
	QTcpSocket *mCurrentConnection {}; // Has ownership in QTcpServer

	constexpr static int port = 10000;
};

}
