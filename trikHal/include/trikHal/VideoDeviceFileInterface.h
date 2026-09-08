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
#include <QtCore/QString>
#include <QtCore/QVector>
#include <cstdint>
#include <cstddef>
#include "trikHal/trikHalDeclSpec.h"

namespace trikHal {

/// A V4L2 capture device: open/start/stop a streaming capture and hand raw
/// frames to consumers via the frameReady() signal. Implementations own the
/// underlying buffers and are created by HardwareAbstractionInterface.
class TRIKHAL_EXPORT VideoDeviceFileInterface : public QObject
{
	Q_OBJECT

public:
	explicit VideoDeviceFileInterface(QObject *parent = nullptr) : QObject(parent) {}
	~VideoDeviceFileInterface() override = default;

	virtual bool open() = 0;

	/// Start capturing. The stream runs until stopStreaming() is called.
	virtual bool startStreaming() = 0;

	/// Lock the exposure to manual, if the device supports it and it has not
	/// been fixed yet. Intended for continuous capture consumers that need a
	/// stable brightness; a still-shot consumer should leave it on auto.
	/// Idempotent and non-blocking.
	virtual void fixExposure() = 0;

	virtual void stopStreaming() = 0;
	virtual void close() = 0;

	/// Return the buffer identified by @p bufferIdx back to the driver (QBUF),
	/// so the next frame can be captured into it.
	virtual void release(uint32_t bufferIdx) = 0;

	virtual uint32_t actualFourcc() const = 0;
	virtual uint32_t bytesPerLine() const = 0;

	/// Configure the device to capture into caller-managed buffers (USERPTR
	/// mode) instead of driver-allocated MMAP buffers. Must be called before
	/// open(). Each entry of @p buffers is a physically contiguous buffer the
	/// VPIF DMA engine writes one frame into; all of them are queued so the
	/// camera streams continuously. Pass an empty vector to use the default
	/// MMAP mode. The default implementation is a no-op (MMAP only).
	virtual void setUserPtrBuffers(const QVector<void *> &buffers, size_t bufferSize)
	{
		Q_UNUSED(buffers);
		Q_UNUSED(bufferSize);
	}

Q_SIGNALS:
	/// Emitted when a frame is captured into buffer @p bufferIdx.  @p data is
	/// the buffer's virtual address (valid until release() is called for that
	/// buffer) and @p size is the frame size in bytes.
	void frameReady(uint32_t bufferIdx, const uint8_t *data, size_t size);
};

} // namespace trikHal
