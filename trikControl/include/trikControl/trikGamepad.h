#pragma once

#include <QtCore/QObject>
#include <QSharedPointer>
#include <QThread>
#include "declSpec.h"

namespace trikControl {

class TcpConnector;

class TRIKCONTROL_EXPORT TrikGamepad : public QObject
{
    Q_OBJECT
public:
    TrikGamepad(int port);

signals:
    void trikGamepad_padUp(int pad);
    void trikGamepad_wheel(int persent);
    void trikGamepad_pad(int pad, int x, int y);
    void trikGamepad_button(int button, int pressed);

private slots:
    void parse(QString message);

private:
    TcpConnector* connector;
    QThread* networkThread;
};

}
