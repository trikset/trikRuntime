/* Copyright 2015 CyberTech Labs Ltd.
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

#include "mspBusAutoDetector.h"

#include <memory>

#include <QtCore/QScopedPointer>

#include <trikHal/hardwareAbstractionInterface.h>
#include <QsLog.h>

#include "i2cCommunicator.h"
#include "mspUsbCommunicator.h"

using namespace trikControl;

PeripheryCommunicatorInterface *MspBusAutoDetector::createCommunicator(const trikKernel::Configurer &configurer,
	trikHal::HardwareAbstractionInterface &hardwareAbstraction)
{
	QLOG_INFO() << "Checking USB periphery communicator for availability";
	std::unique_ptr<trikHal::UsbDeviceInterface> usb(hardwareAbstraction.createUsbDevice());
	if (!usb) {
		QLOG_INFO() << "Using I2C periphery communicator";
		return new I2cCommunicator(configurer, hardwareAbstraction.peripheryI2c());
	}

	QScopedPointer<PeripheryCommunicatorInterface> communicator(new MspUsbCommunicator(std::move(usb)));
	if (communicator->status() == DeviceInterface::Status::permanentFailure) {
		QLOG_INFO() << "Using I2C periphery communicator";
		return new I2cCommunicator(configurer, hardwareAbstraction.peripheryI2c());
	}

	QLOG_INFO() << "Using USB periphery communicator";
	return communicator.take();
}
