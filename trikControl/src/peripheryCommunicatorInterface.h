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

#include <cstddef>

#include <QtCore/QByteArray>

#include "deviceInterface.h"
#include "deviceState.h"

namespace trikControl {

/// Abstract interface for communication with the periphery module over any
/// supported transport (USB, I2C, ...).
class PeripheryCommunicatorInterface : public DeviceInterface
{
public:
	/// Send data to the current device, if it is connected.
	virtual int send(const QByteArray &data) = 0;

	/// Reads data by the given command and returns the result.
	virtual int read(const QByteArray &data) = 0;

	/// Sends several commands as one batch, if the transport supports it.
	/// The default implementation falls back to a single send().
	virtual int sendBatch(const QByteArray &data)
	{
		return send(data);
	}

	/// Maximum payload size of one batch, or 0 when batching is unsupported.
	virtual std::size_t batchMaxPayload() const
	{
		return 0;
	}
};

}