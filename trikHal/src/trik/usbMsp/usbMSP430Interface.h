/* Copyright 2015 Rostislav Varzar
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
 * limitations under the License.
 * Created on: Fabruary 12, 2015
 *	Author: Rostislav Varzar
*/

#pragma once
#include "stdint.h"
#include <QByteArray>

namespace trikHal {

class MspSendMessageInterface;

namespace trik {

	/// Connect to USB MSP430 device
	uint32_t connect_USBMSP();

	/// Disconnect from USB MSP430 device
	uint32_t disconnect_USBMSP();

	/// Send data to MSP430 via USB
	uint32_t send_USBMSP(uint16_t deviceAddress, uint16_t value, bool isWord = false);

	/// Read data from MSP430 via USB
	uint32_t read_USBMSP(uint16_t deviceAddress, uint16_t numberOfBytes);
}
}
