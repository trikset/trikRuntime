#pragma once
#include "i2cDeviceInterface.h"
#include <trikHal/mspI2cInterface.h>

namespace trikControl {

/// Interface extending the main I2c interface with i2ctransfer
class CommonI2cDeviceInterface : public trikHal::MspI2cInterface
{
public:
	/// Perform i2c transfer operation.
	virtual int transfer(const QVector<I2cDeviceInterface::Message> &vector) = 0;
};
}
