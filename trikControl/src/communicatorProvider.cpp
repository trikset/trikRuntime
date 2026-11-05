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

#include <memory>

#include <trikHal/hardwareAbstractionInterface.h>
#include <trikKernel/configurer.h>
#include <trikKernel/exceptions/malformedConfigException.h>
#include <QsLog.h>

#include "i2cCommunicator.h"
#include "nrfDriver.h"
#include "nrfUsbCommunicator.h"

using namespace trikControl;

CommunicatorProvider::CommunicatorProvider(const trikKernel::Configurer &configurer,
	trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mConfigurer(&configurer)
	, mHardwareAbstraction(&hardwareAbstraction)
{
	createUsbCommunicator();
	createI2cCommunicator();
}

CommunicatorProvider::~CommunicatorProvider() = default;

void CommunicatorProvider::createUsbCommunicator()
{
#ifdef TRIK_PERIPHERY_NRF
	try {
		const auto telemetryPeriodStr = mConfigurer->attributeByDevice("nrfUsb", "telemetryPeriodUs");
		const auto isoPacketsPerUrbStr = mConfigurer->attributeByDevice("nrfUsb", "isoPacketsPerUrb");

		bool ok = false;
		const auto telemetryPeriodUs = telemetryPeriodStr.toUInt(&ok, 10);
		if (!ok) {
			QLOG_ERROR() << "CommunicatorProvider: invalid nrfUsb telemetryPeriodUs:" << telemetryPeriodStr;
			return;
		}

		const auto isoPacketsPerUrb = isoPacketsPerUrbStr.toUInt(&ok, 10);
		if (!ok) {
			QLOG_ERROR() << "CommunicatorProvider: invalid nrfUsb isoPacketsPerUrb:" << isoPacketsPerUrbStr;
			return;
		}

		std::unique_ptr<trikHal::UsbDeviceInterface> usb(mHardwareAbstraction->createUsbDevice());
		if (!usb) {
			QLOG_ERROR() << "CommunicatorProvider: createUsbDevice() failed, is /dev/nrf0 present?";
			return;
		}

		usb->setTelemetryPeriodUs(telemetryPeriodUs, isoPacketsPerUrb);

		auto nrf = std::make_unique<NrfUsbCommunicator>(std::move(usb), telemetryPeriodUs);
		if (!nrf->connect()) {
			QLOG_ERROR() << "CommunicatorProvider: NrfUsbCommunicator connect failed";
			return;
		}

		mUsb.reset(nrf.release());
		QLOG_INFO() << "CommunicatorProvider: using USB nRF communicator";
	} catch (const trikKernel::MalformedConfigException &e) {
		QLOG_INFO() << "CommunicatorProvider: nrfUsb is not configured, skipping:" << e.errorMessage();
	}
#else
	QLOG_INFO() << "CommunicatorProvider: USB periphery communicator is not available for this module";
#endif
}

void CommunicatorProvider::createI2cCommunicator()
{
	try {
		mI2c.reset(new I2cCommunicator(*mConfigurer, mHardwareAbstraction->peripheryI2c()));
	} catch (const trikKernel::MalformedConfigException &e) {
		QLOG_INFO() << "CommunicatorProvider: I2C periphery communicator is not configured:"
			    << e.errorMessage();
	}
}

PeripheryDriverInterface *CommunicatorProvider::communicator()
{
	if (!mDriver) {
#ifdef TRIK_PERIPHERY_NRF
		mDriver.reset(new NrfDriver(mUsb.data(), mI2c.data(), nullptr));
#else
		QLOG_ERROR() << "CommunicatorProvider: periphery driver is not implemented for this module";
		return nullptr;
#endif
	}

	return mDriver.data();
}

PeripheryCommunicatorInterface *CommunicatorProvider::peripheryCommunicator()
{
	communicator();

	if (mI2c) {
		return mI2c.data();
	}

	return mUsb.data();
}

trikHal::HardwareAbstractionInterface &CommunicatorProvider::hw()
{
	return *mHardwareAbstraction;
}
