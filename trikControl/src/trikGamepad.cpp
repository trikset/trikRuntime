#include "trikGamepad.h"
#include "tcpConnector.h"
#include <QStringList>

using namespace trikControl;

TrikGamepad::TrikGamepad(int port)
{
    listener = new TcpConnector(port);
    connect(listener, SIGNAL(dataReady(QString)), this, SLOT(parse(QString)));
    connect(&networkThread, SIGNAL(started()), listener, SLOT(startServer()));
    listener->moveToThread(&networkThread);
    networkThread.start();
}

void TrikGamepad::parse(QString message)
{
    QStringList cmd = message.split(" ", QString::SkipEmptyParts);
    QString commandName = cmd.at(0).trimmed();
    if (commandName == "pad")
    {
        int pad = cmd.at(1).trimmed().toInt();
        if (cmd.at(2).trimmed() == "up")
            emit trikGamepad_padUp(pad);
        else
        {
            int x = cmd.at(2).trimmed().toInt();
            int y = cmd.at(3).trimmed().toInt();
            emit trikGamepad_pad(pad, x, y);
        }
    }
    else if (commandName == "btn")
    {
        int but = cmd.at(1).trimmed().toInt();
        emit trikGamepad_button(but, 1);
    }
    else if (commandName == "wheel")
    {
        int perc = cmd.at(1).trimmed().toInt();
        emit trikGamepad_wheel(perc);
    }
    else
    {
        qDebug() << "Unknown command" ;
    }
}

