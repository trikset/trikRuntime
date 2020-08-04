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

#include "trikVariablesServer.h"

#include <QtCore/QJsonDocument>
#include <QtCore/QRegExp>
#include <QtNetwork/QTcpSocket>
#include <QsLog.h>
using namespace trikScriptRunner;

TrikVariablesServer::TrikVariablesServer() :
	mTcpServer(new QTcpServer(this))
{
	connect(mTcpServer.data(), &QTcpServer::newConnection, this, &TrikVariablesServer::onNewConnection);
	if (!mTcpServer->listen(QHostAddress::LocalHost, port)) {
		QLOG_ERROR() << "Failed to open port" << port << "for variables";
	}
}

void TrikVariablesServer::sendHTTPResponse(const QJsonObject &json)
{
	QByteArray jsonBytes = QJsonDocument(json).toJson();

	// TODO: Create other way for endline constant, get rid of define
#define NL "\r\n"
	QString header = "HTTP/1.0 200 OK" NL
					 "Connection: close" NL
					 "Content-type: text/plain, charset=us-ascii" NL
					 "Content-length: " + QString::number(jsonBytes.size()) + NL
					 NL;
#undef NL

	mCurrentConnection->write(header.toLatin1());
	mCurrentConnection->write(jsonBytes);
	mCurrentConnection->close();
}

void TrikVariablesServer::onNewConnection()
{
	// TODO: Object from nextPendingConnection is a child of QTcpServer, so it will be automatically
	// deleted when QTcpServer is destroyed. Maybe it may sense to call "deleteLater" explicitly,
	// to avoid wasting memory.
	mCurrentConnection = mTcpServer->nextPendingConnection();
	connect(mCurrentConnection, &QTcpSocket::readyRead, this, &TrikVariablesServer::processHTTPRequest);
}

void TrikVariablesServer::processHTTPRequest()
{
	// TODO: Make sure, that different connections aren't intersected using mutex or
	// support multiple connections simultaneously
	QStringList list;
	while (mCurrentConnection->canReadLine())
	{
		QString data = QString(mCurrentConnection->readLine());
		list.append(data);
	}

	const QString cleanString = list.join("").remove(QRegExp(R"([\n\t\r])"));
	const QStringList words = cleanString.split(QRegExp("\\s+"), QString::SkipEmptyParts);

	if (words[1] == "/web/") {
		emit getVariables("web");
	}
}
