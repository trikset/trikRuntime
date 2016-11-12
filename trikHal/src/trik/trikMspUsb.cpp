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

#include "trikMspUsb.h"

#include "usbMsp/usbMSP430Defines.h"
#include "usbMsp/usbMSP430Interface.h"

#include <QsLog.h>

using namespace trikHal::trik;

TrikMspUsb::~TrikMspUsb()
{
}

void TrikMspUsb::send(const QByteArray &data)
{
	send_USBMSP(data);
}

int TrikMspUsb::read(const QByteArray &data)
{
	return read_USBMSP(data);
}

bool TrikMspUsb::connect()
{
#ifndef I_UNDERSTAND_ALL_RISKS
	return false;
#else
	// Connect to USB device
	if (connect_USBMSP() == DEVICE_ERROR) {
		QLOG_INFO() << "Failed to open USB device file " << USB_DEV_NAME;
		return false;
	}

	return true;
#endif /* I_UNDERSTAND_ALL_RISKS */
}

void TrikMspUsb::disconnect()
{
	disconnect_USBMSP();
}
