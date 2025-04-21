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

#pragma once

#include <QtCore/QByteArray>

#include "deviceInterface.h"
#include "deviceState.h"

namespace trikControl {

/// Abstract interface for communication with MSP, may be implemented by I2C or USB buses, depending on current MSP
/// configuration.
class MspCommunicatorInterface : public DeviceInterface
{
public:
	/// Send data to current device, if it is connected.
	virtual int send(const QByteArray &data) = 0;

	/// Reads data by given I2C command number and returns the result.
	virtual int read(const QByteArray &data) = 0;

	/// Reads data by given I2C command number and returns the result.
	virtual QVector<uint8_t> readX(const QByteArray &data) = 0;
};

}
