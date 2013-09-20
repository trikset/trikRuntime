/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

#include "trikCommunicator.h"

#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

#include <trikScriptRunner/trikScriptRunner.h>

using namespace trikCommunicator;

TrikCommunicator::TrikCommunicator()
	: mConnection(new QTcpSocket())
	, mRunner(new trikScriptRunner::TrikScriptRunner())
	, mOwnsRunner(true)
{
}


TrikCommunicator::TrikCommunicator(trikScriptRunner::TrikScriptRunner &runner)
	: mConnection(new QTcpSocket())
	, mRunner(&runner)
	, mOwnsRunner(false)
{
}

TrikCommunicator::~TrikCommunicator()
{
	delete mConnection;
	delete mRunner;
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
		mRunner->run(fileContents);
	} else if (command == "stop") {
		mRunner->abort();
		mRunner->run("brick.stop()");
	} else if (command.startsWith("direct")) {
		command.remove(0, QString("direct:").length());
		mRunner->run(command);
	}
}
