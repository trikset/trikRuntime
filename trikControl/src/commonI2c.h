/* Copyright 2025 CyberTech Labs Ltd.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License. */

#pragma once

#include <QtCore/QByteArray>
#include "commonI2cDeviceInterface.h"

namespace trikControl {

/// Implementing an I2c device using linux I2c and I2c_RDRW
class CommonI2c: public CommonI2cDeviceInterface
{
public:

	/// Constructor
	explicit CommonI2c(uint16_t regSize): mRegSize(regSize) {};

	~CommonI2c() override;

	/// Send data to a device.
	int send(const QByteArray &data) override;

	/// Reads data by given I2C command number and returns the result.
	int read(const QByteArray &data) override;

	/// Reads data by given I2C command number and returns the result.
	QVector<uint8_t> readX(const QByteArray &data) override;

	/// Perform I2c transfer operation.
	int transfer(const QVector<I2cDeviceInterface::Message> &vector) override;

	/// Establish I2C connection.
	bool connect(const QString &devicePath, int deviceId) override;

	/// Close I2C Connection.
	void disconnect() override;

private:
	uint16_t mRegSize;
	int mDeviceFileDescriptor = -1;
	ushort mDeviceAddress = 0;
	int write(uchar* writeData__u8, ushort length);

};

}
