#pragma once

#include <QtCore/QByteArray>
#include "commonI2cDeviceInterface.h"

namespace trikControl {

/// Implementing an I2c device using linux I2c and I2c_RDRW
class CommonI2c: public CommonI2cDeviceInterface
{
public:

	/// Constructor
	explicit CommonI2c(uint16_t regSize);

	/// Send data to a device.
	int send(const QByteArray &data) override;

	/// Reads data by given I2C command number and returns the result.
	int read(const QByteArray &data) override;

	/// Reads data by given I2C command number and returns the result.
	QVector<uint8_t> readX(const QByteArray &data) override;

	/// Perform i2c transfer operation
	int transfer(const QVector<I2cDeviceInterface::Message> &vector) override;

	bool connect(const QString &devicePath, int deviceId) override;

	void disconnect() override;

private:
	uint16_t mRegSize;
	int mDeviceFileDescriptor = -1;
	ushort mDeviceAddress = 0;
	int write(uchar* writeData__u8, ushort length);

};

}
