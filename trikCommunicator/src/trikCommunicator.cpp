/* Copyright 2013 Yurii Litvinov
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

#include "trikCommunicator.h"

#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

#include <trikScriptRunner/trikScriptRunner.h>

using namespace trikCommunicator;

TrikCommunicator::TrikCommunicator(QString const &configFilePath)
	: mConnection(new QTcpSocket())
	, mRunner(new trikScriptRunner::TrikScriptRunner(configFilePath))
	, mOwnsRunner(true)
	, mExecutionState(idle)
{
	connect(mRunner, SIGNAL(completed()), this, SLOT(onScriptExecutionCompleted()));
}

TrikCommunicator::TrikCommunicator(trikScriptRunner::TrikScriptRunner &runner)
	: mConnection(new QTcpSocket())
	, mRunner(&runner)
	, mOwnsRunner(false)
	, mExecutionState(idle)
{
	connect(mRunner, SIGNAL(completed()), this, SLOT(onScriptExecutionCompleted()));
}

TrikCommunicator::~TrikCommunicator()
{
	delete mConnection;
	if (mOwnsRunner) {
		delete mRunner;
	}
}

QString TrikCommunicator::readFromFile(QString const &fileName)
{
	QFile file(fileName);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	if (!file.isOpen()) {
		throw "Reading failed";
	}

	QTextStream input;
	input.setDevice(&file);
	input.setCodec("UTF-8");
	QString const result = input.readAll();
	file.close();
	return result;
}

void TrikCommunicator::writeToFile(QString const &fileName, QString const &contents)
{
	QFile file(fileName);
	file.open(QIODevice::WriteOnly | QIODevice::Text);
	if (!file.isOpen()) {
		throw "File open operation failed";
	}

	QTextStream stream(&file);
	stream.setCodec("UTF-8");
	stream << contents;
	file.close();
}

void TrikCommunicator::listen(int const &port)
{
	connect(&mServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
	mServer.listen(QHostAddress::Any, port);
}

void TrikCommunicator::onNewConnection()
{
	qDebug() << "New connection";

	delete mConnection;
	mConnection = mServer.nextPendingConnection();
	connect(mConnection, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
	connect(mConnection, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

void TrikCommunicator::onDisconnected()
{
	qDebug() << "Disconnected";

	mConnection->disconnectFromHost();
}

void TrikCommunicator::onReadyRead()
{
	if (!mConnection->isValid()) {
		return;
	}

	QByteArray const &data = mConnection->readAll();
	QString command(data);

	if (!command.startsWith("keepalive")) {
		// Discard "keepalive" output.
		qDebug() << "Command: " << command;
	}

	if (command.startsWith("file")) {
		command.remove(0, QString("file:").length());
		int const separatorPosition = command.indexOf(':');
		if (separatorPosition == -1) {
			// TODO: Add logging and don't crash server.

			qDebug() << "Malformed 'file' command";

			throw "Malformed command";
		}

		QString const fileName = command.left(separatorPosition);
		QString const fileContents = command.mid(separatorPosition + 1);
		writeToFile(fileName, fileContents);
	} else if (command.startsWith("run")) {
		command.remove(0, QString("run:").length());
		QString const fileContents = readFromFile(command);
		mExecutionState = running;
		mRunner->run(fileContents);
	} else if (command == "stop") {
		mExecutionState = stopping;
		mRunner->abort();
		mRunner->run("brick.stop()");
	} else if (command.startsWith("direct")) {
		command.remove(0, QString("direct:").length());
		mExecutionState = running;
		mRunner->run(command);
	}
}

void TrikCommunicator::onScriptExecutionCompleted()
{
	if (mExecutionState == running) {
		mExecutionState = stopping;
		mRunner->run("brick.stop()");
	} else {
		mExecutionState = idle;
	}
}
