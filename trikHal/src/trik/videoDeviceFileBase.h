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

#include <VideoDeviceFileInterface.h>

#include <QtCore/QScopedPointer>
#include <QtCore/QVector>
#include <atomic>
#include <cstdint>
#include <cstddef>

#include <trikKernel/videoUtils.h>

class QSocketNotifier;

namespace trikHal {

/// Shared V4L2 capture implementation (mmap or USERPTR buffers, notifier-driven
/// dequeue) for the target hardware. Streams start/stop and USB-webcam exposure
/// handling live here; concrete devices only plug in their format negotiation.
class VideoDeviceFileBase : public VideoDeviceFileInterface
{
	Q_OBJECT

public:
	/// Records the requested capture parameters. The device is not touched until
	/// open() is called. @p bufferCount is the number of V4L2 buffers to
	/// allocate in MMAP mode (USERPTR mode uses the caller-provided list); the
	/// default matches the shared DSP input geometry.
	VideoDeviceFileBase(const QString &devicePath, uint32_t width, uint32_t height, uint32_t preferredFourcc,
		uint32_t bufferCount = trikKernel::dspInputBuffersPerRegion, bool isWebcam = false,
		QObject *parent = nullptr);

	/// Closes the device (idempotent).
	~VideoDeviceFileBase() override;

	/// Opens the device, negotiates the format and maps the capture buffers.
	bool open() override;

	/// Queues the mapped buffers and switches the driver to streaming.
	bool startStreaming() override;

	/// See VideoDeviceFileInterface::fixExposure().
	void fixExposure() override;

	/// Switches the driver off streaming and drops the notifier.
	void stopStreaming() override;

	/// Releases every OS resource held by the device. Idempotent.
	void close() override;

	/// Returns the current buffer to the driver (QBUF).
	void release(uint32_t bufferIdx) override;

	uint32_t actualFourcc() const override
	{
		return mActualFourcc;
	}
	uint32_t bytesPerLine() const override
	{
		return mLineLen;
	}

	/// See VideoDeviceFileInterface::setUserPtrBuffers().
	void setUserPtrBuffers(const QVector<void *> &buffers, size_t bufferSize) override;

protected:
	/// Sets the capture format via VIDIOC_S_FMT. May be overridden to add extra
	/// device-specific negotiation on top of the base implementation.
	virtual bool setFormat();

	/// Forwards a dequeued frame to the frameReady() signal.
	virtual void onFrameReady(uint32_t bufferIdx, const uint8_t *data, size_t size);

	/// Apply the default V4L2 control values for a USB (UVC) webcam. Mirrors the
	/// init_webcam() step previously performed by the media-sensor init script:
	/// anti-flicker, fixed white balance and gain. Exposure is left on auto.
	void applyWebcamDefaults();

	/// Set a single V4L2 control on the device.
	bool setControl(uint32_t id, int32_t value);

	/// Requests and maps the mmap capture buffers. Cleans up partial state on
	/// failure, so it is safe to call without relying on the caller to close().
	virtual bool allocateBuffers();

	/// Unmaps every capture buffer.
	virtual void freeBuffers();

	QString mPath;
	uint32_t mWidth;
	uint32_t mHeight;
	uint32_t mRequestedFourcc;
	uint32_t mActualFourcc = 0;
	uint32_t mLineLen = 0;
	uint32_t mSizeImage = 0;
	uint32_t mBufferCount;
	bool mIsWebcam = false;
	bool mExposureFixed = false;
	int mFd = -1;
	/// Whether streaming is on. Atomic because release() may be invoked from a
	/// consumer thread; read there to drop a stale QBUF after streamoff.
	std::atomic<bool> mStreaming {false};

	struct MmapBuf {
		uint8_t *data = nullptr;
		size_t size = 0;
	};
	QVector<MmapBuf> mMmapBufs;

	bool mUseUserPtr = false;
	QVector<void *> mUserPtrBuffers;
	size_t mUserPtrSize = 0;

private Q_SLOTS:
	void onActivated(int fd);

private:
	/// Disable the socket notifier from its own thread. QSocketNotifier is
	/// thread-affine: setEnabled()/destruction from another thread warns and is
	/// a no-op. Runs inline when already on the notifier's thread, otherwise
	/// posts a queued call to it. Used by onActivated() to just park the
	/// notifier (the stream may restart).
	void disableNotifier();

	/// Disable and destroy the socket notifier on its own thread (blocking when
	/// called from a foreign thread). Used by stopStreaming()/close(): the
	/// notifier is never created again for this stream cycle, so it must be
	/// torn down on the thread that owns it.
	void teardownNotifier();

	QScopedPointer<QSocketNotifier> mNotifier;
};

} // namespace trikHal
