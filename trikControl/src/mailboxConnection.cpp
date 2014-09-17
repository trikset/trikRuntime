#include "src/mailboxConnection.h"

#include <QtCore/QDebug>

using namespace trikControl;

MailboxConnection::MailboxConnection()
{
}

void MailboxConnection::connect(QString const &myIp, QString const &targetIp, int targetPort, int myHullNumber)
{
	init(targetIp, targetPort);

	QString const handshake = QString("register:%1:%2").arg(myIp).arg(myHullNumber);
	send(handshake.toLocal8Bit());
}

void MailboxConnection::processData(QString const &data)
{
	if (data.startsWith("register:")) {
		QStringList const parsedString = data.split(":");
		if (parsedString.size() != 3) {
			qDebug() << "Malformed data: " << data;
		} else {
			emit newConnection(parsedString[1], parsedString[2].toInt());
		}
	}
}
