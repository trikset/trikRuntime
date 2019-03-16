#include "gyroscopeindicator.h"

#include <QtCore/QString>

#include <trikControl/gyroSensorInterface.h>

using namespace trikGui;

GyroscopeIndicator::GyroscopeIndicator(trikControl::GyroSensorInterface &gyroscope
        , QWidget *parent)
    : AbstractIndicator(parent)
    , mGyroscope(gyroscope)
    , mNameLabel("Gyroscope")
{
    mNameLabel.setAlignment(Qt::AlignCenter);
    mValueX.setAlignment(Qt::AlignCenter);
    mValueY.setAlignment(Qt::AlignCenter);
    mValueZ.setAlignment(Qt::AlignCenter);

    mLayout.addWidget(&mNameLabel);
    mLayout.addWidget(&mValueX);
    mLayout.addWidget(&mValueY);
    mLayout.addWidget(&mValueZ);
    setLayout(&mLayout);

    setFocusPolicy(Qt::StrongFocus);
}

void GyroscopeIndicator::renew()
{
    QVector<int> value = mGyroscope.read();
    mValueX.setText(QString::number(value[0]));
    mValueY.setText(QString::number(value.takeAt(1)));
    mValueZ.setText(QString::number(value.takeAt(2)));
}
