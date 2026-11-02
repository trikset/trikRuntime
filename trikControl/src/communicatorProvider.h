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

#include "peripheryCommunicatorInterface.h"
#include "peripheryDriverInterface.h"

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikKernel {
class Configurer;
}

namespace trikControl {

/// Holds the shared periphery driver and provides access to the hardware abstraction.
///
/// Device factory functions receive a CommunicatorProvider as their third
/// parameter and select the right resource based on the policy tag:
///
///   CommunicatorPolicy  →  provider.communicator()  (PeripheryDriverInterface)
///   InternalPolicy      →  provider.hw()
class CommunicatorProvider
{
public:
	CommunicatorProvider() = default;

	CommunicatorProvider(const trikKernel::Configurer &configurer,
		trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	~CommunicatorProvider();

	/// Returns the shared periphery driver.
	PeripheryDriverInterface *communicator();

	/// Returns the communicator used by legacy device classes that still send
	/// raw commands. Prefers I2C when both transports are available.
	PeripheryCommunicatorInterface *peripheryCommunicator();

	/// Returns the hardware abstraction (for InternalPolicy).
	trikHal::HardwareAbstractionInterface &hw();

private:
	void createUsbCommunicator();
	void createI2cCommunicator();

	const trikKernel::Configurer *mConfigurer = nullptr;
	trikHal::HardwareAbstractionInterface *mHardwareAbstraction = nullptr;
	QScopedPointer<PeripheryDriverInterface> mDriver;
	QScopedPointer<PeripheryCommunicatorInterface> mUsb;
	QScopedPointer<PeripheryCommunicatorInterface> mI2c;
};

} // namespace trikControl