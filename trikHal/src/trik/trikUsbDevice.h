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

#include <QtCore/QByteArray>
#include <QtCore/QObject>

#include <trikHal/trikHalDeclSpec.h>

#include <array>
#include <atomic>
#include <cstdint>
#include <thread>

namespace trikHal {
namespace trik {

/// Kernel-driver based nRF transport (/dev/nrf0).
///
/// Uses the kernel nrf-usb driver: ISO IN telemetry via poll + read,
/// commands via bulk OUT ioctl (NRF_IO_BULK), vendor control transfers
/// via NRF_IO_CTRL. Endpoints and device ids are handled entirely by the
/// driver.
class TRIKHAL_EXPORT TrikUsbDevice : public UsbDeviceInterface
{
	Q_DISABLE_COPY(TrikUsbDevice)
public:
	TrikUsbDevice();
	~TrikUsbDevice() override;

	bool connect() override;
	void disconnect() override;
	bool isConnected() const override;
	int send(const QByteArray &data) override;
	bool sendBulk(const QByteArray &data) override;
	void setDataCallback(DataCallback callback, void *userData) override;
	void setTelemetryPeriodUs(uint32_t periodUs, u_int32_t isoPacketsPerUrb) override;
	bool controlWrite(uint8_t bRequest, const QByteArray &data, int timeoutMs) override;
	int read(const QByteArray &data) override;

private:
	void eventLoop();

	/// Size of one telemetry frame as sent by the nRF firmware.
	static constexpr int kTelemetrySize = 71;
	static constexpr const char *kDevicePath = "/dev/nrf0";

	int mFd = -1;
	std::atomic<bool> mRunning {false};
	std::thread mEventThread;

	DataCallback mDataCallback = nullptr;
	void *mDataCallbackUserData = nullptr;
};

}
}