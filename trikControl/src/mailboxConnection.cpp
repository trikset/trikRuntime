#include "src/mailboxConnection.h"

#include <QtCore/QStringList>

#include <QtCore/QDebug>

using namespace trikControl;

MailboxConnection::MailboxConnection()
	: trikKernel::Connection(trikKernel::Protocol::messageLength)
{
}

void MailboxConnection::connect(const QHostAddress &targetIp, int targetPort, int myServerPort, int myHullNumber)
{
	init(targetIp, targetPort);

	QString const handshake = QString("register:%1:%2").arg(myServerPort).arg(myHullNumber);
	send(handshake.toUtf8());
}

void MailboxConnection::sendConnectionInfo(QHostAddress const &ip, int port, int hullNumber)
{
	QString const info = QString("connection:%1:%2:%3").arg(ip.toString()).arg(port).arg(hullNumber);
	send(info.toUtf8());
}

void MailboxConnection::sendSelfInfo(int hullNumber)
{
	QString const info = QString("self:%3").arg(hullNumber);
	send(info.toUtf8());
}

void MailboxConnection::processData(QByteArray const &rawData)
{
	QString const data = QString::fromUtf8(rawData);

	if (data.startsWith("register:")) {
		QStringList const parsedString = data.split(":");
		if (parsedString.size() != 3) {
			qDebug() << "Malformed data: " << data;
		} else {
			bool serverPortOk = false;
			int const serverPort = parsedString[1].toInt(&serverPortOk);
			bool hullNumberOk = false;
			int const hullNumber = parsedString[1].toInt(&hullNumberOk);
			if (!serverPortOk || !hullNumberOk) {
				qDebug() << "Malformed data: " << data;
			} else {
				emit newConnection(peerAddress(), peerPort(), serverPort, hullNumber);
			}
		}
	} else if (data.startsWith("connection:")) {
		QStringList const parsedString = data.split(":");
		if (parsedString.size() != 4) {
			qDebug() << "Malformed data: " << data;
		} else {
			bool serverPortOk = false;
			int const serverPort = parsedString[2].toInt(&serverPortOk);
			bool hullNumberOk = false;
			int const hullNumber = parsedString[3].toInt(&hullNumberOk);
			if (!serverPortOk || !hullNumberOk) {
				qDebug() << "Malformed data: " << data;
			} else {
				emit connectionInfo(QHostAddress(parsedString[1]), serverPort, hullNumber);
			}
		}
	} else if (data.startsWith("self:")) {
		QStringList const parsedString = data.split(":");
		if (parsedString.size() != 2) {
			qDebug() << "Malformed data: " << data;
		} else {
			// Self-info. Host and port is to be determined automatically via socket.
			bool hullNumberOk = false;
			int const hullNumber = parsedString[1].toInt(&hullNumberOk);
			if (!hullNumberOk) {
				qDebug() << "Malformed data: " << data;
			} else {
				emit connectionInfo(peerAddress(), peerPort(), hullNumber);
			}
		}
	} else if (data.startsWith("data:")) {
		QString parsedString = data;
		parsedString.remove(0, QString("data:").length());
		emit newData(peerAddress(), peerPort(), parsedString.toUtf8());
	}
}
