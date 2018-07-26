#pragma once

#include <QtCore/QObject>

#include "deviceInterface.h"

#include "declSpec.h"

namespace trikControl {

class TRIKCONTROL_EXPORT I2cDeviceInterface : public QObject, public DeviceInterface
{
    Q_OBJECT

public :
    /// Send data to current device, if it is connected.
    virtual void send(const QByteArray &data) = 0;

    /// Reads data by given I2C command number and returns the result.
    virtual int read(const QByteArray &data) = 0;
};

}
