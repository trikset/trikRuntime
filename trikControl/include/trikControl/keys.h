#pragma once

#include <QObject>
#include <QVector>
#include <QSocketNotifier>
#include <QSharedPointer>

#include "declSpec.h"

namespace trikControl {

class TRIKCONTROL_EXPORT Keys : public QObject
{
    Q_OBJECT

public:
    Keys();
    void init(QString const &keysPath);

public slots:
    void readKeysEvent();

signals:
    void buttonPressed(int code, int value);

private:
    QSharedPointer<QSocketNotifier> mSocketNotifier;
    int mKeysFd;
    int buttonCode;
    int buttonValue;
};

}
