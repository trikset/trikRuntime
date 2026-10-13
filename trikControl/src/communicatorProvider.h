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

#include <QtCore/QScopedPointer>

#include "mspDriver.h"

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikKernel {
class Configurer;
}

namespace trikControl {

class MspUsbCommunicator;
class MspI2cCommunicator;

/// Holds the shared MspDriver and its communicators.
///
/// The driver is created lazily with both communicators: USB when available
/// and I2C as a fallback, so the driver can pick the transport itself.
class CommunicatorProvider
{
public:
	CommunicatorProvider() = default;

	CommunicatorProvider(const trikKernel::Configurer &configurer
			, trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	~CommunicatorProvider();

	/// Returns the shared periphery driver (created lazily on first call).
	PeripheryDriverInterface *communicator();

	/// Returns the hardware abstraction.
	trikHal::HardwareAbstractionInterface &hw();

private:
	const trikKernel::Configurer *mConfigurer = nullptr;
	trikHal::HardwareAbstractionInterface *mHardwareAbstraction = nullptr;
	QScopedPointer<MspDriver> mDriver;
	QScopedPointer<MspUsbCommunicator> mUsb;
	QScopedPointer<MspI2cCommunicator> mI2c;
};

} // namespace trikControl