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

#include "fbOutputInterface.h"

#include <cstddef>
#include <cstdint>

namespace trikHal {
namespace trik {

/// Linux framebuffer output: mmaps /dev/fb0 and memcpys RGB565 frames straight
/// into it, bypassing Qt rendering.
///
/// This is a temporary hack: rendering the DSP frames through Qt does not meet
/// the timing constraints of the CV algorithms, so we write straight into the
/// framebuffer instead. Revisit once the Qt pipeline is fast enough.
class TrikFbOutput : public FbOutputInterface
{
	Q_OBJECT

public:
	/// Creates a closed output. Call open() before the first writeFrame().
	explicit TrikFbOutput(QObject *parent = nullptr);

	/// Unmaps and closes the device if still open.
	~TrikFbOutput() override;

	/// Opens /dev/fb0 and maps it read-write. Safe to call again once open.
	bool open() override;

	/// Unmaps and closes the device. Idempotent, safe to call when closed.
	void close() override;

	/// Whether the device is currently open and mapped.
	bool isOpen() const override;

	/// Copies one 240x240 RGB565 frame straight into the framebuffer. Drops the
	/// frame when not open, @p rgb565 is null, or the mapped framebuffer is
	/// smaller than the frame.
	void writeFrame(const uint8_t *rgb565) override;

	/// Logical width of the written frame (DSP output size).
	uint32_t frameWidth() const override;

	/// Logical height of the written frame (DSP output size).
	uint32_t frameHeight() const override;

private:
	/// Open framebuffer descriptor, -1 when closed.
	int mFd = -1;

	/// Mapped framebuffer, nullptr when closed.
	uint8_t *mMap = nullptr;

	/// Length of the mapped region (== finfo.smem_len when open).
	size_t mMapLen = 0;

	/// Whether the device is currently open and mapped.
	bool mOpen = false;
};

} // namespace trik
} // namespace trikHal
