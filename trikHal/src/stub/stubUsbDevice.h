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

#include "usbDeviceInterface.h"

namespace trikHal {
namespace stub {

/// Stub USB device for desktop builds and tests. Logs operations.
class StubUsbDevice : public UsbDeviceInterface
{
public:
	StubUsbDevice();

	bool connect() override;
	void disconnect() override;
	bool isConnected() const override;
	int send(const QByteArray &data) override;
	bool controlWrite(uint8_t bRequest, const QByteArray &data, int timeoutMs) override;
	void setDataCallback(DataCallback callback, void *userData) override;
	void setTelemetryPeriodUs(uint32_t periodUs, u_int32_t isoPacketsPerUrb) override;
	int read(const QByteArray &data) override;

private:
	bool mConnected = false;
};

}
}
