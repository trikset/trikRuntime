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

#include "mspI2cCommunicator.h"
#include "configurerHelper.h"

using namespace trikControl;

Encoder::Encoder(const QString &port, const trikKernel::Configurer &configurer, MspCommunicatorInterface &communicator)
	: mCommunicator(communicator)
	, mInvert(configurer.attributeByPort(port, "invert") == "false")
	, mState("Encoder on" + port)
{
	mI2cCommandNumber = ConfigurerHelper::configureInt(configurer, mState, port, "i2cCommandNumber");
	mPassedTicks= ConfigurerHelper::configureInt(configurer, mState, port, "ticks");
	mPassedDegrees= ConfigurerHelper::configureInt(configurer, mState, port, "degrees");

	if (mPassedTicks == 0) {
		QLOG_ERROR() << "'ticks' parameter can not be 0";
		mState.fail();
	}

	mState.ready();
}

void Encoder::reset()
{
	if (status() == DeviceInterface::Status::ready) {
		QByteArray command(3, '\0');
		command[0] = static_cast<char>(mI2cCommandNumber & 0xFF);
		command[1] = static_cast<char>((mI2cCommandNumber >> 8) & 0xFF);
		command[2] = static_cast<char>(0x00);

		mCommunicator.send(command);
	}
}

Encoder::Status Encoder::status() const
{
	return combine(mCommunicator, mState.status());
}

int Encoder::read()
{
	return readRawData() * mPassedDegrees / mPassedTicks * (mInvert ? -1 : 1);
}

int Encoder::readRawData()
{
	if (status() == DeviceInterface::Status::ready) {
		QByteArray command(3, '\0');
		command[0] = static_cast<char>(mI2cCommandNumber & 0xFF);
		command[1] = static_cast<char>((mI2cCommandNumber >> 8) & 0xFF);
		command[2] = static_cast<char>(0x00);

		return mCommunicator.read(command);
	} else {
		return 0;
	}
}
