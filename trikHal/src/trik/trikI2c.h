/* Copyright 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include "mspI2cInterface.h"

namespace trikHal {
namespace trik {

/// Real implementation of I2C bus communicator.
class TrikI2c : public MspI2cInterface
{
public:
	/// Constructor
	TrikI2c() = default;
	~TrikI2c() override;

	int send(const QByteArray &data) override;
	int read(const QByteArray &data) override;
	QVector<uint8_t> readX(const QByteArray &data) override;
	bool connect(const QString &devicePath, int deviceId) override;
	int transfer(const QVector<MspI2cInterface::Message> &vector) override;
	void disconnect() override;

private:
	/// Low-level descriptor of I2C device file.
	int mDeviceFileDescriptor = -1;
};

}
}
