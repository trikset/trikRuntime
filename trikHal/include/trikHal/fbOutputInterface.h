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

#include <QtCore/QObject>
#include "trikHal/trikHalDeclSpec.h"

namespace trikHal {

/// Abstraction over the linux framebuffer for direct RGB565 output.
///
/// Intended for DSP video display: low-overhead memcpy straight into
/// the framebuffer mmap region, bypassing Qt rendering entirely.
///
/// Threading: all methods are expected to be called from the DSP thread
/// (the same thread that owns the DspServer), except the constructor.
class TRIKHAL_EXPORT FbOutputInterface : public QObject
{
	Q_OBJECT

public:
	explicit FbOutputInterface(QObject *parent = nullptr) : QObject(parent) {}
	~FbOutputInterface() override = default;

	/// Open /dev/fb0, mmap, and prepare for writing.
	/// @return true on success.
	virtual bool open() = 0;

	/// Unmap and close the device. Safe to call even if not open.
	virtual void close() = 0;

	/// @return true if the framebuffer is currently open and mapped.
	virtual bool isOpen() const = 0;

	/// Write an RGB565 image directly into the framebuffer.
	/// Silently drops if !isOpen() or pointer is null.
	/// @param rgb565  pointer to 240x240 RGB565 pixel data (BUFFER_SIZE_FOR_FB bytes).
	virtual void writeFrame(const uint8_t *rgb565) = 0;

	/// Logical dimensions of the written frame (hardcoded 240x240 for DSP output).
	virtual uint32_t frameWidth() const = 0;
	virtual uint32_t frameHeight() const = 0;
};

} // namespace trikHal
