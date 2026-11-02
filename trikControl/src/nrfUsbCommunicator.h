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

#include <array>
#include <memory>

#include "deviceState.h"
#include "peripheryCommunicatorInterface.h"
#include "nrfUsbProtocol.h"

namespace trikHal {
class UsbDeviceInterface;
}

namespace trikControl {

/// Shared pointer to an immutable telemetry snapshot (USB path only).
///
/// A snapshot is never modified after publication: the USB thread allocates a
/// brand new instance per telemetry frame and the reader always sees a fully
/// consistent set of values.  Shared ownership lets a reader keep using the
/// snapshot it grabbed even after a newer frame has been published.
using SnapshotPtr = std::shared_ptr<const TelemetrySnapshot>;

/// USB transport for the nRF52833 periphery module.
///
/// Implements PeripheryCommunicatorInterface on top of the bulk endpoints of a
/// UsbDeviceInterface.  Outgoing commands are wrapped into a CommHeader frame
/// (see nrfUsbProtocol.h) and written to the bulk OUT endpoint — either a
/// single fire-and-forget command via send() or a pre-packed batch via
/// sendBatch().  Incoming periodic telemetry frames are parsed on the USB
/// callback thread and published as lock-free snapshots, so any thread can
/// poll the latest() values without blocking.
///
/// Telemetry publication is lock-free: publishSnapshot() builds a new
/// immutable snapshot and atomically stores it into mLatestSnapshot, while
/// readers atomically load it.  No reader ever sees a partially updated
/// snapshot, and the shared_ptr keeps the snapshot it read alive.
class NrfUsbCommunicator : public QObject, public PeripheryCommunicatorInterface
{
	Q_OBJECT

public:
	/// Creates a communicator over the given USB device, taking ownership of it.
	/// @param usb the opened-or-lazy USB device used for bulk and control I/O.
	/// @param telemetryPeriodUs requested telemetry streaming period in
	///        microseconds; 0 disables periodic frames.  No I/O happens until
	///        connect().
	NrfUsbCommunicator(std::unique_ptr<trikHal::UsbDeviceInterface> usb, uint32_t telemetryPeriodUs);

	/// Disconnects from the USB device (idempotent).
	~NrfUsbCommunicator() override;

	/// Registers the data callback, opens the USB connection and requests
	/// telemetry streaming at the configured period via a control transfer.
	/// @return true on success; on failure the device is marked failed and
	///         false is returned.
	bool connect();

	/// Tears the connection down and removes the data callback.  Safe to call
	/// even if not connected.
	void disconnect();

	/// Sends one command as a fire-and-forget frame (kReqIdFireAndForget).
	/// @return the number of payload bytes written, or 0 if the device is not
	///         ready or the bulk write failed.
	int send(const QByteArray &data) override;

	/// Sends a pre-packed payload as a single batch frame (kReqIdBatch).
	/// Unlike send() the caller already serialised one or more commands, so no
	/// framing of the content is performed here.  The payload must not exceed
	/// batchMaxPayload() bytes.
	/// @return the number of payload bytes written, or 0 on failure.
	int sendBatch(const QByteArray &payload) override;

	/// Maximum payload bytes a single batch frame can carry.  Larger batches
	/// must be split by the caller (see NrfDriver::flush()).
	std::size_t batchMaxPayload() const override
	{
		return 256;
	}

	/// Reads a telemetry port value from the latest snapshot.
	/// @param data two little-endian bytes holding the port table index
	///        (mirrors the firmware port table, see nrfUsbProtocol.h).
	/// @return the last value published for that port, or 0 if the device is
	///         not ready or the index is out of range.
	int read(const QByteArray &data) override;

	/// Device state (ready/fail/off).  See DeviceInterface::Status.
	Status status() const override;

	/// Returns the latest published telemetry snapshot, or the initial empty
	/// snapshot if no frame has arrived yet.  Thread-safe and non-blocking:
	/// safe to call from any thread while frames are being published.
	SnapshotPtr latest() const;

private:
	/// Wraps @p data in a CommHeader frame with the given reqId and writes it
	/// to the bulk OUT endpoint.  Returns payload length on success, 0 on
	/// failure or if the device is not ready.
	int sendFramed(const QByteArray &data, uint8_t reqId);

	/// C trampoline forwarding the USB data callback into handleData().
	static void onUsbData(const QByteArray &data, void *userData);

	/// Parses a bulk IN buffer into CommHeader frames and publishes any
	/// telemetry frames found in it.  Runs on the USB callback thread.
	void handleData(const QByteArray &data);

	/// Builds a new immutable snapshot and atomically publishes it as the
	/// latest one.  Runs on the USB callback thread.
	void publishSnapshot(uint64_t timestampUs,
		const std::array<int32_t, nrfUsbProtocol::kTelemetryPortCount> &values);

	/// Underlying USB device, owned by this communicator.
	std::unique_ptr<trikHal::UsbDeviceInterface> mUsb;

	/// Telemetry period requested from the module on connect(), in microseconds.
	uint32_t mTelemetryPeriodUs;

	/// Latest published telemetry snapshot, atomically replaced on every frame.
	SnapshotPtr mLatestSnapshot {std::make_shared<TelemetrySnapshot>()};

	/// Communicator state machine (ready/fail/off).
	DeviceState mState;
};

}
