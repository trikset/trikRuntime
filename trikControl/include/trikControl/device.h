#pragma once

#include <QtCore/QObject>
#include <QtCore/QSocketNotifier>
#include <QtCore/QSharedPointer>
#include <QtCore/QVector>

#include "declSpec.h"

namespace trikControl {

class TRIKCONTROL_EXPORT Device : public QObject
{
    Q_OBJECT

public:
    Device();
    void init(int min, int max, QString const &controlFile);

public slots:
    QVector<int> readTilts();
private slots:
    void readFile();

private:
    QSharedPointer<QSocketNotifier> mSocketNotifier;
    int mDeviceFd;
    QVector<int> tilts;
    int mMax;
    int mMin;
};

}
