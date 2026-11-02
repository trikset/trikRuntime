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
#include <QtCore/QObject>

#include <cstdint>

#include <trikHal/trikHalDeclSpec.h>

namespace trikHal {

/// Common USB transport for a periphery module.
///
/// The interface is intentionally transport-agnostic: an implementation may
/// use a serial link, a vendor protocol, a kernel driver, etc. Optional
/// operations have default no-op implementations so a concrete device only
/// implements what it actually supports.
class TRIKHAL_EXPORT UsbDeviceInterface
{
	Q_DISABLE_COPY(UsbDeviceInterface)
public:
	/// Callback for incoming data from the device.
	using DataCallback = void (*)(const QByteArray &data, void *userData);

	UsbDeviceInterface() = default;

	virtual ~UsbDeviceInterface() = default;

	/// Open the device and prepare it for I/O. Returns true on success.
	virtual bool connect() = 0;

	/// Stop I/O and release the device.
	virtual void disconnect() = 0;

	/// Returns true if the device is connected.
	virtual bool isConnected() const = 0;

	/// Sends one command to the device.
	/// @return 0 on success, a negative value on error.
	virtual int send(const QByteArray &data) = 0;

	/// Sends a batch of commands, if the transport supports it.
	/// Returns false by default.
	virtual bool sendBulk(const QByteArray &data)
	{
		Q_UNUSED(data)
		return false;
	}

	/// Registers a callback for unsolicited incoming data, if supported.
	virtual void setDataCallback(DataCallback callback, void *userData)
	{
		Q_UNUSED(callback)
		Q_UNUSED(userData)
	}

	/// Configures telemetry timing, if supported.
	virtual void setTelemetryPeriodUs(uint32_t periodUs, uint32_t isoPacketsPerUrb)
	{
		Q_UNUSED(periodUs)
		Q_UNUSED(isoPacketsPerUrb)
	}

	/// Sends a vendor control request, if supported. Returns false by default.
	// NOLINTNEXTLINE(google-default-arguments)
	virtual bool controlWrite(uint8_t bRequest, const QByteArray &data, int timeoutMs = 1000)
	{
		Q_UNUSED(bRequest)
		Q_UNUSED(data)
		Q_UNUSED(timeoutMs)
		return false;
	}

	/// Reads a value for the given command. Returns 0 if unsupported.
	virtual int read(const QByteArray &data) = 0;
};

}
