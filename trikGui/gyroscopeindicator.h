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
class GyroSensorInterface;
}

namespace trikGui {

/// Widget that shows current gyroscope reading.
class GyroscopeIndicator : public AbstractIndicator
{
    Q_OBJECT

public:
    /// Constructor.
    /// @param gyroscope - gyroscope which we will read.
    /// @param parent - parent of this widget in Qt widget parent-child system.
    GyroscopeIndicator(trikControl::GyroSensorInterface &gyroscope, QWidget *parent = 0);

public slots:
    void renew() override;

private:
    trikControl::GyroSensorInterface &mGyroscope;

    QHBoxLayout mLayout;
    QLabel mNameLabel;
    QLabel mValueX;
    QLabel mValueY;
    QLabel mValueZ;
};
}
