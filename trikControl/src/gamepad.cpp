#include "gamepad.h"

#include <QtCore/QStringList>

#include "tcpConnector.h"

using namespace trikControl;

Gamepad::Gamepad(int port)
	: mListener(new TcpConnector(port))
{
	connect(mListener.data(), SIGNAL(dataReady(QString)), this, SLOT(parse(QString)));
	connect(&mNetworkThread, SIGNAL(started()), mListener.data(), SLOT(startServer()));
	mListener->moveToThread(&mNetworkThread);
	mNetworkThread.start();
}

Gamepad::~Gamepad()
{
}

void Gamepad::parse(QString const &message)
{
	QStringList const cmd = message.split(" ", QString::SkipEmptyParts);
	QString const commandName = cmd.at(0).trimmed();
	if (commandName == "pad") {
		int const padId = cmd.at(1).trimmed().toInt();
		if (cmd.at(2).trimmed() == "up") {
			emit padUp(padId);
		} else {
			int const x = cmd.at(2).trimmed().toInt();
			int const y = cmd.at(3).trimmed().toInt();
			emit pad(padId, x, y);
		}
	} else if (commandName == "btn") {
		int const buttonCode = cmd.at(1).trimmed().toInt();
		emit button(buttonCode, 1);
	} else if (commandName == "wheel") {
		int const perc = cmd.at(1).trimmed().toInt();
		emit wheel(perc);
	} else {
		qDebug() << "Unknown command" ;
	}
}
