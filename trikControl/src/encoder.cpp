//ip 192.168.0.108
#include <encoder.h>
#include <QDebug>

#include "i2cCommunicator.h"

#define PAR_TO_RAD 0.03272492

using namespace trikControl;

Encoder::Encoder(I2cCommunicator &communicator, int jbx)
    : mCommunicator(communicator),
      mJbx(jbx),
      mData(0.0)
{
}

void Encoder::reset()
{
    QByteArray command(2, '\0');
    switch (mJbx)
    {
        case 2:
            command[0] = static_cast<char>(0x31);
            break;
        case 3:
            command[0] = static_cast<char>(0x32);
            break;
        case 4:
            command[0] = static_cast<char>(0x33);
            break;
    }
    command[1] = static_cast<char>(0x00);

    mCommunicator.send(command);
}

float Encoder::get()
{
    int data = 0;
    QByteArray command(2, '\0');

    switch (mJbx)
    {
        case 2:
            command[0] = static_cast<char>(0x31);
            break;
        case 3:
            command[0] = static_cast<char>(0x32);
            break;
        case 4:
            command[0] = static_cast<char>(0x33);
            break;
    }

    data = mCommunicator.read(command);
    mData = PAR_TO_RAD * data;

    return mData;
}

