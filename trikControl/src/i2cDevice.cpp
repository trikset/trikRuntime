/* Copyright 2018 Aleksey Fefelov and CyberTech Labs Ltd.
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

// FIXME: Transfer ownership of the i2c object to another class, such as Brick or TrikHardwareAbstraction.
#include <trikHal/mspI2cInterface.h>
#include "i2cDevice.h"

using namespace trikControl;

I2cDevice::I2cDevice(const trikKernel::Configurer &configurer, trikHal::MspI2cInterface *i2c, int bus, int address)
	: mState("I2cDevice")
	, mInterface(i2c)
	, mCommunicator(new MspI2cCommunicator(configurer, *i2c, bus, address))
{
	mState.ready();
}

I2cDevice::~I2cDevice() {}

I2cDevice::Status I2cDevice::status() const
{
	return combine(*mCommunicator, mState.status());
}

void I2cDevice::send8(uint16_t reg, uint8_t value)
{
	if (status() == DeviceInterface::Status::ready) {
		mCommunicator->send(reg, value, false);
	}
}

void I2cDevice::send16(uint16_t reg, uint16_t value)
{
	if (status() == DeviceInterface::Status::ready) {
		mCommunicator->send(reg, value, true);
	}
}

int I2cDevice::read8(uint16_t reg)
{
	return read(reg, 1);
}

int I2cDevice::read16(uint16_t reg)
{
	return read(reg, 2);
}

int I2cDevice::read32(uint16_t reg)
{
	return read(reg, 4);
}

int I2cDevice::read(uint16_t reg, uint16_t number)
{
	return mCommunicator->read(reg, number).toInt();
}
