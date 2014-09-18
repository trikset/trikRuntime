#include "src/mailboxConnection.h"

#include <QtCore/QStringList>

#include <QtCore/QDebug>

using namespace trikControl;

MailboxConnection::MailboxConnection()
{
}

void MailboxConnection::onConnect(QString const &targetIp, int targetPort, int myHullNumber)
{
	onInit(QHostAddress(targetIp), targetPort);

	QString const handshake = QString("register:%1").arg(myHullNumber);
	onSend(handshake.toLocal8Bit());
}

void MailboxConnection::onUpdateConnectionInfo(QHostAddress const &ip, int port, int hullNumber)
{
	QString const info = QString("update:%1:%2:%3").arg(ip.toString()).arg(port).arg(hullNumber);
	onSend(info.toLocal8Bit());
}

void MailboxConnection::processData(QString const &data)
{
	if (data.startsWith("register:")) {
		QStringList const parsedString = data.split(":");
		if (parsedString.size() != 2) {
			qDebug() << "Malformed data: " << data;
		} else {
			/// @todo Check for correctness of toInt()
			emit newConnection(peerAddress(), peerPort(), parsedString[1].toInt());
		}
	} else if (data.startsWith("update:")) {
		QStringList const parsedString = data.split(":");
		if (parsedString.size() != 4) {
			qDebug() << "Malformed data: " << data;
		} else {
			if (parsedString[2] != "-1") {
				/// @todo Check for correctness of toInt()
				emit updateConnectionInfo(QHostAddress(parsedString[1])
						, parsedString[2].toInt()
						, parsedString[3].toInt());
			} else {
				// Self-info. Host and port is to be determined automatically via socket.
				/// @todo Check for correctness of toInt()
				emit updateConnectionInfo(peerAddress()
						, peerPort()
						, parsedString[3].toInt());
			}
		}
	} else if (data.startsWith("data:")) {
		QString parsedString = data;
		parsedString.remove(0, QString("data:").length());
		qDebug() << "emitting" << parsedString;
		emit newData(peerAddress(), peerPort(), parsedString.toLocal8Bit());
	}
}
