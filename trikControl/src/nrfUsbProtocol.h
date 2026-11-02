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

#include <array>
#include <cstddef>
#include <cstdint>

namespace trikControl {

/// Protocol constants for nRF52833 periphery module USB communication.
/// Mirror of trik-nrf52833/nrfUsbProtocol.h.
namespace nrfUsbProtocol {

/// CommHeader wire format (LE, packed).
#pragma pack(push, 1)
struct CommHeader {
	uint8_t type;
	uint8_t flags;
	uint8_t reqId;
	uint8_t seqId;
	uint16_t len;
};
#pragma pack(pop)

static_assert(sizeof(CommHeader) == 6, "CommHeader must be 6 bytes");

/// type
constexpr uint8_t kTypePeriphery = 0x01;

/// reqId
constexpr uint8_t kReqIdTelemetry = 0x00;
constexpr uint8_t kReqIdFireAndForget = 0xFE;
constexpr uint8_t kReqIdBatch = 0xFD;

/// usb vendor control requests
constexpr uint8_t kUsbRawReqSetTelemetry = 0x02;
constexpr std::size_t kTelemetryControlDataSize = 5;

/// payload constants
constexpr std::size_t kHeaderSize = 6;
constexpr std::size_t kTelemetryFixed = 1 + 8; // control byte + ts_us u64
constexpr std::size_t kTelemetryPortCount = 14;
constexpr std::size_t kTelemetryPayloadSize = 1 + 8 + kTelemetryPortCount * 4; // 65
constexpr std::size_t kTelemetryFrameSize = kHeaderSize + kTelemetryPayloadSize; // 71

/// Port table indices (mirror nRF52833 firmware port table).

} // namespace nrfUsbProtocol

/// Lock-free telemetry snapshot.
struct TelemetrySnapshot {
	uint64_t timestampUs = 0;
	std::array<int32_t, nrfUsbProtocol::kTelemetryPortCount> values {};
	uint32_t generation = 0;
};

} // namespace trikControl