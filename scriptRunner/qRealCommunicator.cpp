#include "qRealCommunicator.h"

#include <QtCore/QFile>
#include <QtCore/QDebug>

using namespace scriptRunner;

int const bufferSize = 1000;

QRealCommunicator::QRealCommunicator()
	: mConnection(new QTcpSocket())
{
}

QString QRealCommunicator::readFromFile(QString const &fileName)
{
	QFile file(fileName);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	if (!file.isOpen()) {
		return "";
	}

	QTextStream input;
	input.setDevice(&file);
	input.setCodec("UTF-8");
	QString const result = input.readAll();
	file.close();
	return result;
}

void QRealCommunicator::writeToFile(QString const &fileName, QString const &contents)
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

void QRealCommunicator::listen(int const &port)
{
	connect(&mServer, SIGNAL(newConnection()), this, SLOT(onNewConnection()));
	mServer.listen(QHostAddress::Any, port);
}

void QRealCommunicator::onNewConnection()
{
	qDebug() << "New connection";
	delete mConnection;
	mConnection = mServer.nextPendingConnection();
	connect(mConnection, SIGNAL(disconnected()), this, SLOT(onDisconnected()));
	connect(mConnection, SIGNAL(readyRead()), this, SLOT(onReadyRead()));
}

void QRealCommunicator::onDisconnected()
{
	qDebug() << "Disconnected";
	mConnection->disconnectFromHost();
}

void QRealCommunicator::onReadyRead()
{
	if (!mConnection->isValid()) {
		return;
	}

	QByteArray data = mConnection->readAll();
	QString command(data);

	if (command != "keepalive") {
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
		mRunner.run(fileContents);
	} else if (command == "stop") {
		mRunner.abort();
		mRunner.run("brick.stop()");
	} else if (command.startsWith("direct")) {
		command.remove(0, QString("direct:").length());
		mRunner.run(command);
	}
}
