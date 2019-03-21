#include "accelerometerindicator.h"

#include <QtCore/QString>
#include <trikControl/vectorSensorInterface.h>

using namespace trikGui;

AccelerometerIndicator::AccelerometerIndicator(trikControl::VectorSensorInterface &accelerometer
        , QWidget *parent)
    : AbstractIndicator(parent)
    , mAccelerometer(accelerometer)
    , mNameLabel("Accelerometer")
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

void AccelerometerIndicator::renew()
{
    QVector<int> value = mAccelerometer.read();
    mValueX.setText(QString::number(value[0]));
    mValueY.setText(QString::number(value.takeAt(1)));
    mValueZ.setText(QString::number(value.takeAt(2)));
}
