/* Copyright 2013 - 2015 Matvey Bryksin, Yurii Litvinov and CyberTech Labs Ltd.
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

#include "encoder.h"

#include <trikKernel/configurer.h>
#include <QsLog.h>

#include "communicatorCommand.h"
#include "configurerHelper.h"
#include "peripheryDriverInterface.h"

using namespace trikControl;

Encoder::Encoder(const QString &port, const trikKernel::Configurer &configurer, PeripheryDriverInterface &driver)
	: mDriver(driver)
	, mInvert(configurer.attributeByPort(port, "invert") == "false")
	, mState("Encoder on" + port)
	, mPort(port)
{
	mPassedTicks = ConfigurerHelper::configureInt(configurer, mState, port, "ticks");
	mPassedDegrees = ConfigurerHelper::configureInt(configurer, mState, port, "degrees");

	if (mPassedTicks == 0) {
		QLOG_ERROR() << "'ticks' parameter can not be 0";
		mState.fail();
	}

	mState.ready();
}

void Encoder::reset()
{
	if (status() == DeviceInterface::Status::ready) {
		CommunicatorCommand command;
		command.device = communicatorCommand::kDevicePort;
		command.portName = mPort;
		command.operation = communicatorCommand::kPortReset;
		mDriver.send(command);
	}
}

Encoder::Status Encoder::status() const
{
	return combine(mDriver, mState.status());
}

int Encoder::read()
{
	return readRawData() * mPassedDegrees / mPassedTicks * (mInvert ? -1 : 1);
}

int Encoder::readRawData()
{
	if (status() == DeviceInterface::Status::ready) {
		return mDriver.readAsync(mPort);
	} else {
		return 0;
	}
}
