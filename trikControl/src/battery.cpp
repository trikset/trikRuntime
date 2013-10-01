#include <battery.h>

#include <QDebug>

#include "i2cCommunicator.h"

using namespace trikControl;

Battery::Battery(I2cCommunicator &communicator)
    : mCommunicator(communicator)
{
}

float Battery::readVoltage()
{
    QByteArray command(1, '\0');
    command[0] = static_cast<char>(0x26);

    int parrot = mCommunicator.read(command);

    float res = ((float) parrot / 1023.0) * 3.3 * (7.15 + 2.37) / 2.37;
    qDebug("battery V: %f", res);
    return res;
}
