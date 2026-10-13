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

#include "communicatorProvider.h"

#include <trikHal/hardwareAbstractionInterface.h>
#include <trikKernel/configurer.h>
#include <QsLog.h>

#include "mspBusAutoDetector.h"
#include "mspI2cCommunicator.h"
#include "mspUsbCommunicator.h"

using namespace trikControl;

CommunicatorProvider::CommunicatorProvider(const trikKernel::Configurer &configurer
		, trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mConfigurer(&configurer)
	, mHardwareAbstraction(&hardwareAbstraction)
{
}

CommunicatorProvider::~CommunicatorProvider() = default;

PeripheryDriverInterface *CommunicatorProvider::communicator()
{
	if (!mDriver) {
		mUsb.reset(MspBusAutoDetector::createUsbCommunicator(*mHardwareAbstraction));
		mI2c.reset(MspBusAutoDetector::createI2cCommunicator(*mConfigurer, *mHardwareAbstraction));
		mDriver.reset(new MspDriver(mUsb.data(), mI2c.data()));
	}

	return mDriver.data();
}

trikHal::HardwareAbstractionInterface &CommunicatorProvider::hw()
{
	return *mHardwareAbstraction;
}