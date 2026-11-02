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

#include "nrfUsbCommunicator.h"

#include <cstring>

#include <trikHal/usbDeviceInterface.h>
#include <trikKernel/timeVal.h>
#include <QsLog.h>

#include "communicatorCommand.h"

using namespace trikControl;

namespace {

/// Little-endian 64-bit read of the telemetry frame's timestamp (ns, from the
/// nRF52833 RTC subsystem).
uint64_t readLe64(const uint8_t *p)
{
	return static_cast<uint64_t>(p[0]) | (static_cast<uint64_t>(p[1]) << 8) | (static_cast<uint64_t>(p[2]) << 16)
	       | (static_cast<uint64_t>(p[3]) << 24) | (static_cast<uint64_t>(p[4]) << 32)
	       | (static_cast<uint64_t>(p[5]) << 40) | (static_cast<uint64_t>(p[6]) << 48)
	       | (static_cast<uint64_t>(p[7]) << 56);
}

/// Little-endian signed 32-bit read of one telemetry port value.
int32_t readLe32(const uint8_t *p)
{
	return static_cast<int32_t>(static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8)
				    | (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24));
}

} // namespace

NrfUsbCommunicator::NrfUsbCommunicator(std::unique_ptr<trikHal::UsbDeviceInterface> usb, uint32_t telemetryPeriodUs)
	: mUsb(std::move(usb))
	, mTelemetryPeriodUs(telemetryPeriodUs)
	, mState("NrfUsbCommunicator")
{
}

NrfUsbCommunicator::~NrfUsbCommunicator()
{
	disconnect();
}

bool NrfUsbCommunicator::connect()
{
	mUsb->setDataCallback(&NrfUsbCommunicator::onUsbData, this);
	if (!mUsb->connect()) {
		QLOG_ERROR() << "NrfUsbCommunicator: USB connect failed";
		mState.fail();
		return false;
	}

	{
		QByteArray ctrlData(static_cast<int>(nrfUsbProtocol::kTelemetryControlDataSize), '\0');
		ctrlData[0] = static_cast<char>(mTelemetryPeriodUs & 0xFF);
		ctrlData[1] = static_cast<char>((mTelemetryPeriodUs >> 8) & 0xFF);
		ctrlData[2] = static_cast<char>((mTelemetryPeriodUs >> 16) & 0xFF);
		ctrlData[3] = static_cast<char>((mTelemetryPeriodUs >> 24) & 0xFF);
		ctrlData[4] = static_cast<char>(1);
		if (!mUsb->controlWrite(nrfUsbProtocol::kUsbRawReqSetTelemetry, ctrlData)) {
			QLOG_WARN() << "NrfUsbCommunicator: failed to set telemetry period via control endpoint";
		}
	}

	mState.ready();
	QLOG_INFO() << "NrfUsbCommunicator: connected, telemetry period" << mTelemetryPeriodUs << "us";
	return true;
}

void NrfUsbCommunicator::disconnect()
{
	mUsb->setDataCallback(nullptr, nullptr);
	mUsb->disconnect();
	mState.off();
}

int NrfUsbCommunicator::send(const QByteArray &data)
{
	return sendFramed(data, nrfUsbProtocol::kReqIdFireAndForget);
}

int NrfUsbCommunicator::sendBatch(const QByteArray &payload)
{
	return sendFramed(payload, nrfUsbProtocol::kReqIdBatch);
}

int NrfUsbCommunicator::sendFramed(const QByteArray &data, uint8_t reqId)
{
	if (!mState.isReady()) {
		return 0;
	}
	QByteArray frame(static_cast<int>(nrfUsbProtocol::kHeaderSize) + data.size(), '\0');
	auto *hdr = reinterpret_cast<nrfUsbProtocol::CommHeader *>(frame.data());
	hdr->type = nrfUsbProtocol::kTypePeriphery;
	hdr->reqId = reqId;
	hdr->len = static_cast<uint16_t>(data.size());
	frame.replace(static_cast<int>(nrfUsbProtocol::kHeaderSize), data.size(), data);
	// All outgoing commands go through the bulk OUT endpoint, both regular
	// and batch ones.
	bool ok = mUsb->sendBulk(frame);
	return ok ? data.size() : 0;
}

int NrfUsbCommunicator::read(const QByteArray &data)
{
	if (!mState.isReady()) {
		return 0;
	}

	if (data.size() >= 2) {
		const auto portIdx =
			static_cast<uint16_t>(static_cast<uint8_t>(data[0]) | (static_cast<uint8_t>(data[1]) << 8));
		const auto snap = latest();
		if (snap && portIdx < static_cast<uint16_t>(snap->values.size())) {
			return snap->values[portIdx];
		}
	}

	return 0;
}

NrfUsbCommunicator::Status NrfUsbCommunicator::status() const
{
	return mState.status();
}

SnapshotPtr NrfUsbCommunicator::latest() const
{
	return std::atomic_load(&mLatestSnapshot);
}

void NrfUsbCommunicator::onUsbData(const QByteArray &data, void *userData)
{
	auto *self = static_cast<NrfUsbCommunicator *>(userData);
	self->handleData(data);
}

// Bulk IN buffers may carry several CommHeader frames back-to-back (e.g. a
// telemetry frame plus a control echo), so the handler walks the buffer frame
// by frame.  A telemetry frame is recognised by its type/reqId and the leading
// kDeviceControl byte; its fixed payload is a 8-byte timestamp followed by
// kTelemetryPortCount 32-bit port values (all little-endian).
void NrfUsbCommunicator::handleData(const QByteArray &data)
{
	const auto *bytes = reinterpret_cast<const uint8_t *>(data.constData());
	std::size_t offset = 0;
	const auto totalLen = static_cast<std::size_t>(data.size());

	while (offset + nrfUsbProtocol::kHeaderSize <= totalLen) {
		nrfUsbProtocol::CommHeader hdr {};
		std::memcpy(&hdr, bytes + offset, nrfUsbProtocol::kHeaderSize);
		const std::size_t frameLen = static_cast<std::size_t>(hdr.len) + nrfUsbProtocol::kHeaderSize;
		if (offset + frameLen > totalLen) {
			break;
		}

		const bool isTelemetry =
			hdr.type == nrfUsbProtocol::kTypePeriphery && hdr.reqId == nrfUsbProtocol::kReqIdTelemetry
			&& hdr.len >= nrfUsbProtocol::kTelemetryFixed
			&& bytes[offset + nrfUsbProtocol::kHeaderSize] == communicatorCommand::kDeviceControl;

		if (isTelemetry) {
			const auto *payload = bytes + offset + nrfUsbProtocol::kHeaderSize;
			const uint64_t tsUs = readLe64(payload + 1);
			std::array<int32_t, nrfUsbProtocol::kTelemetryPortCount> values {};
			for (std::size_t i = 0; i < nrfUsbProtocol::kTelemetryPortCount; ++i) {
				values[i] = readLe32(payload + nrfUsbProtocol::kTelemetryFixed + i * 4);
			}
			publishSnapshot(tsUs, values);
		}

		offset += frameLen;
	}
}

void NrfUsbCommunicator::publishSnapshot(uint64_t timestampUs,
	const std::array<int32_t, nrfUsbProtocol::kTelemetryPortCount> &values)
{
	auto snapshot = std::make_shared<TelemetrySnapshot>();
	snapshot->timestampUs = timestampUs;
	snapshot->values = values;
	snapshot->generation = std::atomic_load(&mLatestSnapshot)->generation + 1;
	std::atomic_store(&mLatestSnapshot, SnapshotPtr(std::move(snapshot)));
}
