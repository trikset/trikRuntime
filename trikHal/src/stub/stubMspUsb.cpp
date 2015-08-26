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

#include "stubMspUsb.h"

#include <QsLog.h>

using namespace trikHal::stub;

void StubMspUsb::send(const QByteArray &data)
{
	QLOG_INFO() << "Sending thru MSP USB stub" << data;
}

int StubMspUsb::read(const QByteArray &data)
{
	QLOG_INFO() << "Reading from MSP USB stub" << data;
	return 0;
}

bool StubMspUsb::connect()
{
	QLOG_INFO() << "Connecting to MSP USB stub";
	return true;
}

void StubMspUsb::disconnect()
{
	QLOG_INFO() << "Disconnecting from MSP USB stub";
}
