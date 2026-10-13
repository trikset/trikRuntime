/* Copyright 2013 - 2015 Matvey Bryksin, Yurii Litvinov and CyberTech Labs Ltd.
 * and CyberTech Labs Ltd.
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

#include <QtCore/QString>

#include "communicatorCommand.h"
#include "configurerHelper.h"
#include "deviceState.h"
#include "encoderInterface.h"
#include "peripheryDriverInterface.h"
#include "transports/policies.h"
#include "transports/transportTraits.h"

#include <trikKernel/configurer.h>
#include <QsLog.h>

namespace trikControl {

/// Implementation of encoder for real robot, parametrized by the transport
/// policy (the device is served by the periphery communicator or by the
/// on-module hardware directly).
template<typename Policy>
class Encoder;

template<>
struct SupportsCommunicator<Encoder> : std::true_type {};

/// Encoder served over the periphery communicator.
template<>
class Encoder<CommunicatorPolicy> : public EncoderInterface
{
public:
	/// Constructor.
	/// @param port - port on which this encoder is configured.
	/// @param configurer - configurer object containing preparsed XML files with encoder parameters.
	/// @param driver - periphery module driver used to query encoder.
	Encoder(const QString &port, const trikKernel::Configurer &configurer, PeripheryDriverInterface *driver)
		: mDriver(driver)
		, mInvert(configurer.attributeByPort(port, "invert") == "false")
		, mState("Encoder on " + port)
	{
		mI2cCommandNumber = ConfigurerHelper::configureInt(configurer, mState, port, "i2cCommandNumber");
		mPassedTicks = ConfigurerHelper::configureInt(configurer, mState, port, "ticks");
		mPassedDegrees = ConfigurerHelper::configureInt(configurer, mState, port, "degrees");

		if (mPassedTicks == 0) {
			QLOG_ERROR() << "'ticks' parameter can not be 0";
			mState.fail();
		} else {
			mState.ready();
		}
	}

	Status status() const override
	{
		return mDriver ? combine(*mDriver, mState.status()) : mState.status();
	}

	int read() override
	{
		if (status() != DeviceInterface::Status::ready) {
			return 0;
		}

		const int raw = readRawData();
		return raw * mPassedDegrees / mPassedTicks * (mInvert ? -1 : 1);
	}

	int readRawData() override
	{
		if (status() != DeviceInterface::Status::ready) {
			return 0;
		}

		CommunicatorCommand command;
		command.device = static_cast<uint8_t>(mI2cCommandNumber & 0xFF);

		// The MSP usb module uses a matching of the required action by the length of the QByteArray.
		// The last byte is added for processing compatibility
		// TODO: rewrite MspUSB module
		QByteArray data(3, '\0');
		data[0] = static_cast<char>(0x04);
		command.data = data;

		return mDriver->read(command);
	}

	void reset() override
	{
		if (status() != DeviceInterface::Status::ready) {
			return;
		}

		CommunicatorCommand command;
		command.device = static_cast<uint8_t>(mI2cCommandNumber & 0xFF);
		command.data = QByteArray(1, '\0');

		mDriver->send(command);
	}

private:
	PeripheryDriverInterface *mDriver;
	int mI2cCommandNumber = 0;
	int mPassedTicks = 0;
	int mPassedDegrees = 0;
	const bool mInvert;
	DeviceState mState;
};

} // namespace trikControl
