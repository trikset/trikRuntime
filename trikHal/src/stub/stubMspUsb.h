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

#include "mspUsbInterface.h"
#include "stubUsbDevice.h"

namespace trikHal {
namespace stub {

/// MSP USB stub, implemented as a thin wrapper over StubUsbDevice.
class StubMspUsb : public MspUsbInterface
{
public:
	int send(const QByteArray &data) override;
	int read(const QByteArray &data) override;
	bool connect() override;
	void disconnect() override;
	bool isConnected() const override;

private:
	StubUsbDevice mDevice;
};

}
}
