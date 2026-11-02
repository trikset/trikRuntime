/* Copyright 2026 CyberTech Labs Ltd.
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

#include <memory>

#include <QtCore/QMutex>
#include <QtCore/QString>

#include "i2cCommunicatorInterface.h"
#include "deviceState.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class PeripheryI2cInterface;
}

namespace trikControl {

/// Communicator with the periphery module over I2C.
/// Implements I2cCommunicatorInterface for environments without USB.
class I2cCommunicator : public I2cCommunicatorInterface
{
public:
	I2cCommunicator(const trikKernel::Configurer &configurer, trikHal::PeripheryI2cInterface &i2c);

	I2cCommunicator(const trikKernel::Configurer &configurer, trikHal::PeripheryI2cInterface &i2c, uint8_t bus,
		uint8_t deviceId);

	I2cCommunicator(const trikKernel::Configurer &configurer,
		std::unique_ptr<trikHal::PeripheryI2cInterface> ownedI2c);

	~I2cCommunicator() override;

	int send(const QByteArray &data) override;
	int read(const QByteArray &data) override;

	/// Reads raw bytes via I2C (used by I2cDevice).
	QVector<uint8_t> readX(const QByteArray &data) override;

	Status status() const override;

private:
	void disconnect();

	QMutex mLock;
	trikHal::PeripheryI2cInterface *mI2c;
	std::unique_ptr<trikHal::PeripheryI2cInterface> mOwnedI2c;
	DeviceState mState;
};

}
