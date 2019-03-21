#pragma once

#include <QtCore/qglobal.h>

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
    #include <QtGui/QHBoxLayout>
    #include <QtGui/QLabel>
    #include <QtGui/QDial>
#else
    #include <QtWidgets/QHBoxLayout>
    #include <QtWidgets/QLabel>
    #include <QtWidgets/QDial>
#endif

#include "abstractIndicator.h"

namespace trikControl {
class VectorSensorInterface;
}

namespace trikGui {

/// Widget that shows current accelerometer reading.
class AccelerometerIndicator : public AbstractIndicator
{
    Q_OBJECT

public:
    /// Constructor.
    /// @param accelerometer - accelerometer which we will read.
    /// @param parent - parent of this widget in Qt widget parent-child system.
    AccelerometerIndicator(trikControl::VectorSensorInterface &accelerometer, QWidget *parent = 0);

public slots:
    void renew() override;

private:
    trikControl::VectorSensorInterface &mAccelerometer;

    QHBoxLayout mLayout;
    QLabel mNameLabel;
    QLabel mValueX;
    QLabel mValueY;
    QLabel mValueZ;
};
}
