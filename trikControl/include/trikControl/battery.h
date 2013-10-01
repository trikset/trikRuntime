#pragma once

#define I2C_SLAVE	0x0703	/* Use this slave address */
#define I2C_SLAVE_FORCE	0x0706	/* Use this slave address, even if it */
#define I2C_SMBUS	0x0720	/* SMBus transfer */
#define ADDRESS 0x48

#include <QtCore/QObject>

#include "declSpec.h"

namespace trikControl {

class I2cCommunicator;

class TRIKCONTROL_EXPORT Battery : public QObject
{
    Q_OBJECT
public:
    Battery(I2cCommunicator &communicator);

public slots:
    float readVoltage();

private slots:

private:
    I2cCommunicator &mCommunicator;
    int mData;

};

}
