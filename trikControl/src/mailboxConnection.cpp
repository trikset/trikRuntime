#include "src/mailboxConnection.h"

#include <QtCore/QStringList>

#include <QtCore/QDebug>

using namespace trikControl;

MailboxConnection::MailboxConnection()
{
	qDebug() << "MailboxConnection::MailboxConnection()";
}

void MailboxConnection::onConnect(const QHostAddress &targetIp, int targetPort, int myServerPort, int myHullNumber)
{
	qDebug() << "MailboxConnection::onConnect()";

	onInit(targetIp, targetPort);

	QString const handshake = QString("register:%1:%2").arg(myServerPort).arg(myHullNumber);
	onSend(handshake.toLocal8Bit());
}

void MailboxConnection::onConnectionInfo(QHostAddress const &ip, int port, int hullNumber)
{
	QString const info = QString("connection:%1:%2:%3").arg(ip.toString()).arg(port).arg(hullNumber);
	onSend(info.toLocal8Bit());
}

void MailboxConnection::onSelfInfo(int hullNumber)
{
	QString const info = QString("self:%3").arg(hullNumber);
	onSend(info.toLocal8Bit());
}

void MailboxConnection::processData(QByteArray const &rawData)
{
	QString const data(rawData);

	qDebug() << "Received data:" << data << "thread " << thread();

	if (data.startsWith("register:")) {
		QStringList const parsedString = data.split(":");
		if (parsedString.size() != 3) {
			qDebug() << "Malformed data: " << data;
		} else {
			/// @todo Check for correctness of toInt()
			emit newConnection(peerAddress(), peerPort(), parsedString[1].toInt(), parsedString[2].toInt());
		}
	} else if (data.startsWith("connection:")) {
		QStringList const parsedString = data.split(":");
		if (parsedString.size() != 4) {
			qDebug() << "Malformed data: " << data;
		} else {
			/// @todo Check for correctness of toInt()
			emit connectionInfo(QHostAddress(parsedString[1])
					, parsedString[2].toInt()
					, parsedString[3].toInt());
		}
	} else if (data.startsWith("self:")) {
		QStringList const parsedString = data.split(":");
		if (parsedString.size() != 2) {
			qDebug() << "Malformed data: " << data;
		} else {
			// Self-info. Host and port is to be determined automatically via socket.
			/// @todo Check for correctness of toInt()
			emit connectionInfo(peerAddress()
					, peerPort()
					, parsedString[1].toInt());
		}
	} else if (data.startsWith("data:")) {
		QString parsedString = data;
		parsedString.remove(0, QString("data:").length());
		emit newData(peerAddress(), peerPort(), parsedString.toLocal8Bit());
	}
}
