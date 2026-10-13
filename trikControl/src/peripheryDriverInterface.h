/* Copyright 2026 CyberTech Labs Ltd.
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
#include <QtCore/QString>

#include "communicatorCommand.h"
#include "deviceInterface.h"

namespace trikControl {

/// Interface of the periphery module driver.
///
/// The driver decides itself which protocol and transport (USB, I2C, ...) to
/// use for each call: the client does not care which bus a request goes over.
class PeripheryDriverInterface : public DeviceInterface
{
public:
	~PeripheryDriverInterface() override = default;

	/// Send a request directly, using the protocol defined by the driver.
	/// The call is not asynchronous and guarantees that the protocol hardware
	/// module on the other side either acknowledged the request or returned
	/// an error.
	virtual void send(const CommunicatorCommand &command) = 0;

	/// Deferred batched send of multiple requests. The driver chooses the
	/// protocol according to its own logic. Implies asynchronous send, for
	/// example on a timer.
	virtual void sendBatch(const CommunicatorCommand &command) = 0;

	/// Request-response, using the protocol defined by the driver.
	/// The call is not asynchronous and guarantees that the protocol hardware
	/// module on the other side either acknowledged the request or returned
	/// an error.
	virtual int read(const CommunicatorCommand &command) = 0;

	/// Asynchronous request-response read of a port: the call itself is not
	/// blocking, but the response is obtained immediately, not deferred. The
	/// driver chooses the protocol and the port mapping according to its own
	/// logic.
	virtual int readAsync(const QString &port) = 0;
};

} // namespace trikControl
