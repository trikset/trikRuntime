/* Copyright 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include "stubMspI2c.h"
#include <QsLog.h>

using namespace trikHal::stub;

void StubMspI2C::send(uint16_t deviceAddress, uint16_t value, bool isWord)
{
	QLOG_INFO() << "Sending thru MSP I2C stub" << deviceAddress << value << isWord;
}

QVariant StubMspI2C::read(uint16_t deviceAddress, uint16_t numberOfBytes)
{
	QLOG_INFO() << "Reading from MSP I2C stub" << deviceAddress << numberOfBytes;
	return 0;
}

bool StubMspI2C::connect(const QString &devicePath, int deviceId)
{
	QLOG_INFO() << "Connecting to MSP I2C stub, devicePath:" << devicePath << "deviceId" << deviceId;
	return true;
}

void StubMspI2C::disconnect()
{
	QLOG_INFO() << "Disconnecting from MSP I2C stub";
}
