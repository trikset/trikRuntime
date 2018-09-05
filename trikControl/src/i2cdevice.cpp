#include "i2cdevice.h"

using namespace trikControl;

I2cDevice::I2cDevice(int bus, int adress
					 , trikControl::MspCommunicatorInterface &communicator)
	: mState("I2cDevice")
	, mCommunicator(communicator)


{
	mAdress = adress;
	mBus = bus;

	mState.ready();
}


I2cDevice::Status I2cDevice::status() const
{
	return combine(mCommunicator, mState.status());
}

void I2cDevice :: send(const QByteArray &data)
{
	mCommunicator.send(data);
}

int I2cDevice :: read(const QByteArray &data)
{
	return mCommunicator.read(data);
}
