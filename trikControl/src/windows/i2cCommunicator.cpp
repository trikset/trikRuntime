/* Copyright 2013 Yurii Litvinov
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

/// @file Stub for I2C communication to make it compilable under Windows. Shall not work here, of course.

#include "src/i2cCommunicator.h"

using namespace trikControl;

I2cCommunicator::I2cCommunicator(QString const &devicePath, int deviceId)
	: mDevicePath(devicePath)
	, mDeviceId(deviceId)
{
}

I2cCommunicator::~I2cCommunicator()
{
}

void I2cCommunicator::connect()
{
}

void I2cCommunicator::send(QByteArray const &data)
{
	Q_UNUSED(data);
}

void I2cCommunicator::disconnect()
{
}

int I2cCommunicator::read(QByteArray const &data)
{
	Q_UNUSED(data);
	return 0;
}
