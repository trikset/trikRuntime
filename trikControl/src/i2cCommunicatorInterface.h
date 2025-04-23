#pragma once

#include <QtCore/QByteArray>
#include "i2cDeviceInterface.h"
#include "mspCommunicatorInterface.h"

namespace trikControl {

class I2cCommunicatorInterface : public MspCommunicatorInterface
{
public:
	I2cCommunicatorInterface() = default;

	virtual ~I2cCommunicatorInterface() = default;

	/// Reads data by given I2C command number and returns the result.
	virtual QVector<uint8_t> readX(const QByteArray &data) = 0;

	/// Reads data by given I2C command number and returns the result as QVector.
	virtual int transfer(const QVector<I2cDeviceInterface::Message> &vector) = 0;
};

}
