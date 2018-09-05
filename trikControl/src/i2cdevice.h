#pragma once

#include "i2cdeviceinterface.h"
#include "mspI2cCommunicator.h"

namespace trikControl{

/// Abstract i2c device
class I2cDevice : public I2cDeviceInterface
{
	Q_OBJECT

public:
	I2cDevice(int bus, int adress
			  , trikControl::MspCommunicatorInterface &communicator);

	Status status() const override;

public slots:
	/// Send data to current device, if it is connected.
	void send(const QByteArray &data) override;

	/// Reads data by given I2C command number and returns the result.
	int read(const QByteArray &data) override;

private:
	int mBus;
	int mAdress;
	DeviceState mState;
	MspCommunicatorInterface &mCommunicator;
};

}
