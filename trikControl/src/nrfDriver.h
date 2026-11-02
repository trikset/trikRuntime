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
#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtCore/QVector>

#include "communicatorCommand.h"
#include "deviceInterface.h"
#include "peripheryCommunicatorInterface.h"
#include "peripheryDriverInterface.h"

namespace trikControl {

/// Unified bus driver for the nRF52833 periphery module.
///
/// Clients submit a CommunicatorCommand with a physical port name; the driver
/// resolves the port to its wire address and serialises the payload with USB
/// CommHeader framing.  Immediate sends go out right away; deferred batched
/// commands queue and flush on a periodic timer.
class NrfDriver : public QObject, public PeripheryDriverInterface
{
	Q_OBJECT

public:
	NrfDriver(PeripheryCommunicatorInterface *usb, PeripheryCommunicatorInterface *i2c, QObject *parent = nullptr);
	~NrfDriver() override;

	// PeripheryDriverInterface
	void send(const CommunicatorCommand &cmd) override;
	void sendBatch(const CommunicatorCommand &cmd) override;
	int read(const CommunicatorCommand &cmd) override;
	int readAsync(const QString &port) override;

	/// Sends one command through the I2C communicator. Returns the transport
	/// result, or -1 when the I2C path is unavailable or the port is unknown.
	int sendI2c(const CommunicatorCommand &cmd);

	DeviceInterface::Status status() const override;

private:
	/// Resolves a physical port name (e.g. "A1") to its wire address index.
	int portIndex(const QString &portName) const;

	/// Serialises a command to wire bytes [device, portIdx, operation, data...].
	QByteArray serialize(const CommunicatorCommand &cmd) const;

	/// Queues a command for deferred batched send (used by sendBatch).
	void enqueue(const CommunicatorCommand &cmd);

private slots:
	void flush();

private:
	PeripheryCommunicatorInterface *mUsb;
	PeripheryCommunicatorInterface *mI2c;

	QVector<QByteArray> mBatchQueue;
	QTimer mFlushTimer;

	static constexpr int kDefaultFlushMs = 5;
};

} // namespace trikControl