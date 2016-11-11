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

#include <QtCore/QScopedPointer>

#include <trikHal/hardwareAbstractionInterface.h>
#include <QsLog.h>

#include "mspI2cCommunicator.h"
#include "mspUsbCommunicator.h"

using namespace trikControl;

MspCommunicatorInterface *MspBusAutoDetector::createCommunicator(const trikKernel::Configurer &configurer
		, trikHal::HardwareAbstractionInterface &hardwareAbstraction)
{
	QLOG_INFO() << "Checking USB MSP communicator for availability";
	QScopedPointer<MspUsbCommunicator> communicator(new MspUsbCommunicator(hardwareAbstraction.mspUsb()));
	if (communicator->status() == DeviceInterface::Status::permanentFailure) {
		QLOG_INFO() << "Using I2C MSP communicator";
		return new MspI2cCommunicator(configurer, hardwareAbstraction.mspI2c());
	}

	QLOG_INFO() << "Using USB MSP communicator";
	return communicator.take();
}
