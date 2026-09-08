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

#include "videoDeviceFileBase.h"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <linux/videodev2.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <QtCore/QSocketNotifier>
#include <QtCore/QThread>
#include <QsLog.h>

using namespace trikHal;

VideoDeviceFileBase::VideoDeviceFileBase(const QString &devicePath, // NOLINT(modernize-pass-by-value)
	uint32_t width, uint32_t height, uint32_t preferredFourcc, uint32_t bufferCount, bool isWebcam, QObject *parent)
	: VideoDeviceFileInterface(parent)
	, mPath(devicePath)
	, mWidth(width)
	, mHeight(height)
	, mRequestedFourcc(preferredFourcc)
	, mBufferCount(bufferCount)
	, mIsWebcam(isWebcam)
{
}

VideoDeviceFileBase::~VideoDeviceFileBase()
{
	close();
}

bool VideoDeviceFileBase::open()
{
	if (mFd >= 0)
		return true;

	mFd = ::open(qPrintable(mPath), O_RDWR | O_NONBLOCK | O_CLOEXEC);
	if (mFd < 0) {
		QLOG_ERROR() << "VideoDeviceFileBase: open" << mPath << "failed:" << strerror(errno);
		return false;
	}

	v4l2_capability cap = {};
	if (ioctl(mFd, VIDIOC_QUERYCAP, &cap) == 0) {
		const auto caps = (cap.capabilities & V4L2_CAP_DEVICE_CAPS) ? cap.device_caps : cap.capabilities;
		constexpr unsigned needed = V4L2_CAP_STREAMING | V4L2_CAP_VIDEO_CAPTURE;
		if ((caps & needed) != needed) {
			close();
			QLOG_ERROR() << "VideoDeviceFileBase:" << mPath << "missing streaming capture caps";
			return false;
		}
	} else {
		QLOG_WARN() << "VideoDeviceFileBase: QUERYCAP failed:" << strerror(errno);
	}

	if (!setFormat() || !allocateBuffers()) {
		close();
		return false;
	}

	// The analog ov7670 cameras (ports named "video*") are initialized over I2C
	// by the kernel driver's reinit node; every other device (the USB webcam)
	// gets its default V4L2 controls applied here, on the very first open,
	// before streaming starts.
	if (mIsWebcam) {
		applyWebcamDefaults();
	}

	QLOG_INFO() << "VideoDeviceFileBase:" << mPath << "opened, format" << Qt::hex << mActualFourcc;
	return true;
}

void VideoDeviceFileBase::applyWebcamDefaults()
{
	// 50 Hz anti-flicker, then fix the white balance and gain. Exposure is left
	// on auto for now; it is locked to manual by fixExposure() only after the
	// camera has streamed and the auto-exposure has had time to stabilize.
	setControl(V4L2_CID_POWER_LINE_FREQUENCY, V4L2_CID_POWER_LINE_FREQUENCY_50HZ);
	setControl(V4L2_CID_AUTO_WHITE_BALANCE, 0);
	setControl(V4L2_CID_WHITE_BALANCE_TEMPERATURE, 4000);
	setControl(V4L2_CID_GAIN, 0);
}

void VideoDeviceFileBase::fixExposure()
{
	if (!mIsWebcam || mExposureFixed)
		return;

	// Lock the auto-exposure to manual right after streaming starts, so the
	// brightness doesn't drift with the scene. The old init script waited 1s
	// for stabilization, but on UVC devices the auto-exposure settles within
	// a few frames — by the time we reach this call (after startStreaming)
	// the exposure is already stable enough.
	setControl(V4L2_CID_EXPOSURE_AUTO, V4L2_EXPOSURE_MANUAL);
	mExposureFixed = true;
}

bool VideoDeviceFileBase::setControl(uint32_t id, int32_t value)
{
	v4l2_control ctrl = {};
	ctrl.id = id;
	ctrl.value = value;

	if (ioctl(mFd, VIDIOC_S_CTRL, &ctrl) < 0) {
		QLOG_WARN() << "VideoDeviceFileBase:" << mPath << "failed to set control 0x" << Qt::hex << id << "to"
			    << Qt::dec << value << ':' << strerror(errno);
		return false;
	}

	return true;
}

void VideoDeviceFileBase::close()
{
	teardownNotifier();

	if (mStreaming)
		stopStreaming();

	freeBuffers();

	if (mFd >= 0) {
		::close(mFd);
		mFd = -1;
	}

	mUseUserPtr = false;
	mUserPtrBuffers.clear();
	mUserPtrSize = 0;
	mExposureFixed = false;

	QLOG_INFO() << "VideoDeviceFileBase:" << mPath << "closed";
}

bool VideoDeviceFileBase::setFormat()
{
	v4l2_format fmt = {};
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = mWidth;
	fmt.fmt.pix.height = mHeight;
	fmt.fmt.pix.pixelformat = mRequestedFourcc;
	fmt.fmt.pix.field = V4L2_FIELD_NONE;

	QLOG_INFO() << "VideoDeviceFileBase:" << mPath << "requesting format" << Qt::hex << fmt.fmt.pix.pixelformat
		    << Qt::dec << fmt.fmt.pix.width << 'x' << fmt.fmt.pix.height;

	if (ioctl(mFd, VIDIOC_TRY_FMT, &fmt) < 0) {
		QLOG_WARN() << "VideoDeviceFileBase: TRY_FMT failed:" << strerror(errno);
	}

	if (ioctl(mFd, VIDIOC_S_FMT, &fmt) < 0) {
		QLOG_ERROR() << "VideoDeviceFileBase: S_FMT failed:" << strerror(errno);
		return false;
	}

	mActualFourcc = fmt.fmt.pix.pixelformat;
	mWidth = fmt.fmt.pix.width;
	mHeight = fmt.fmt.pix.height;
	mLineLen = fmt.fmt.pix.bytesperline;
	mSizeImage = fmt.fmt.pix.sizeimage;

	QLOG_INFO() << "VideoDeviceFileBase:" << mPath << "negotiated format" << Qt::hex << mActualFourcc << Qt::dec
		    << mWidth << 'x' << mHeight << "bytesperline" << mLineLen << "sizeimage" << fmt.fmt.pix.sizeimage;

	// USB webcams (UVC) default to 15 fps at 320×240 YUYV unless the frame
	// interval is set explicitly.  Request 30 fps (33333 us per frame).
	if (mIsWebcam) {
		v4l2_streamparm parm = {};
		parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (ioctl(mFd, VIDIOC_G_PARM, &parm) == 0 && (parm.parm.capture.capability & V4L2_CAP_TIMEPERFRAME)) {
			parm.parm.capture.timeperframe.numerator = 1;
			parm.parm.capture.timeperframe.denominator = 30;
			if (ioctl(mFd, VIDIOC_S_PARM, &parm) < 0) {
				QLOG_WARN() << "VideoDeviceFileBase:" << mPath << "failed to set 30 fps";
			} else {
				QLOG_INFO() << "VideoDeviceFileBase:" << mPath << "set 30 fps";
			}
		}
	}

	return true;
}

void VideoDeviceFileBase::onFrameReady(uint32_t bufferIdx, const uint8_t *data, size_t size)
{
	emit frameReady(bufferIdx, data, size);
}

void VideoDeviceFileBase::release(uint32_t bufferIdx)
{
	// A release() deferred past a stopStreaming()/startStreaming() cycle (e.g. a
	// frame that was in flight when the sensor was stopped) would otherwise
	// re-QBUF a buffer that startStreaming() has already queued, corrupting the
	// driver's buffer queue and stalling capture. Drop it when not streaming.
	if (!mStreaming)
		return;

	if (bufferIdx >= static_cast<uint32_t>(mMmapBufs.size()))
		return;

	v4l2_buffer buf = {};
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.index = bufferIdx;

	if (mUseUserPtr) {
		buf.memory = V4L2_MEMORY_USERPTR;
		buf.m.userptr = reinterpret_cast<unsigned long>(mMmapBufs[static_cast<int>(bufferIdx)].data);
		buf.length = mMmapBufs[static_cast<int>(bufferIdx)].size;
	} else {
		buf.memory = V4L2_MEMORY_MMAP;
	}

	if (ioctl(mFd, VIDIOC_QBUF, &buf) < 0) {
		QLOG_ERROR() << "VideoDeviceFileBase:" << mPath << "QBUF buffer idx" << bufferIdx
			     << "failed:" << strerror(errno);
	}

	// The notifier stays enabled throughout streaming: with multiple buffers
	// queued, the camera keeps filling the free slots while the DSP processes
	// the dequeued ones, so the level-triggered notifier fires again on the
	// next filled buffer without explicit re-arming.
}

void VideoDeviceFileBase::onActivated(int fd)
{
	Q_UNUSED(fd);

	v4l2_buffer buf = {};
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = mUseUserPtr ? V4L2_MEMORY_USERPTR : V4L2_MEMORY_MMAP;

	if (ioctl(mFd, VIDIOC_DQBUF, &buf) < 0) {
		if (errno == EAGAIN) {
			// No filled buffer yet: level-triggered notifier re-arms on its own.
			return;
		}
		// A fatal error (e.g. ENODEV after the camera was torn down) would
		// otherwise keep re-firing the notifier in a busy loop. Disable it; the
		// stream is re-armed by the next startStreaming().
		QLOG_ERROR() << "VideoDeviceFileBase: DQBUF failed:" << strerror(errno);
		disableNotifier();
		return;
	}

	if (buf.index >= static_cast<decltype(buf.index)>(mMmapBufs.size())) {
		QLOG_ERROR() << "VideoDeviceFileBase: invalid buffer index" << buf.index;
		return;
	}

	const auto *data = mMmapBufs[static_cast<int>(buf.index)].data;
	onFrameReady(buf.index, data, buf.bytesused);
}

void VideoDeviceFileBase::disableNotifier()
{
	if (!mNotifier) {
		return;
	}
	// QSocketNotifier is thread-affine: setEnabled() (and the destructor, which
	// calls it when the notifier is enabled) from another thread only warns and
	// does nothing. Run it on the notifier's own thread to be safe.
	if (QThread::currentThread() == mNotifier->thread()) {
		mNotifier->setEnabled(false);
	} else {
		const auto notifier = mNotifier.data();
		QMetaObject::invokeMethod(notifier, [notifier]() { notifier->setEnabled(false); },
			Qt::QueuedConnection);
	}
}

void VideoDeviceFileBase::teardownNotifier()
{
	if (!mNotifier) {
		return;
	}
	// The notifier is created and owned on the manager thread (where
	// startStreaming() runs). Disabling OR destroying it from a foreign thread
	// makes QSocketNotifier warn and the disable become a no-op, so both steps
	// must happen on the notifier's own thread.
	if (QThread::currentThread() == mNotifier->thread()) {
		mNotifier->setEnabled(false);
		mNotifier.reset();
	} else {
		QMetaObject::invokeMethod(mNotifier.data(), [this]() {
			mNotifier->setEnabled(false);
			mNotifier.reset();
		}, Qt::BlockingQueuedConnection);
	}
}

void VideoDeviceFileBase::setUserPtrBuffers(const QVector<void *> &buffers, size_t bufferSize)
{
	mUserPtrBuffers = buffers;
	mUserPtrSize = bufferSize;
	mUseUserPtr = !buffers.isEmpty();
}

bool VideoDeviceFileBase::allocateBuffers()
{
	v4l2_requestbuffers req = {};
	req.count = mUseUserPtr ? static_cast<uint32_t>(mUserPtrBuffers.size()) : mBufferCount;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = mUseUserPtr ? V4L2_MEMORY_USERPTR : V4L2_MEMORY_MMAP;

	if (ioctl(mFd, VIDIOC_REQBUFS, &req) < 0) {
		QLOG_ERROR() << "VideoDeviceFileBase: REQBUFS failed:" << strerror(errno);
		return false;
	}

	if (req.count == 0) {
		QLOG_ERROR() << "VideoDeviceFileBase: REQBUFS returned 0 buffers";
		return false;
	}

	const uint32_t count = mUseUserPtr ? static_cast<uint32_t>(mUserPtrBuffers.size())
	                                   : std::min<uint32_t>(req.count, mBufferCount);
	mMmapBufs.reserve(static_cast<int>(count));

	if (mUseUserPtr) {
		// The VPIF DMA engine writes a full frame (negotiated sizeimage) into
		// each caller-managed buffer. Reject buffers smaller than one frame,
		// otherwise the DMA would overrun the caller's memory.
		if (mUserPtrSize < mSizeImage) {
			QLOG_ERROR() << "VideoDeviceFileBase: USERPTR buffer too small, need" << mSizeImage << "have"
				     << mUserPtrSize;
			return false;
		}

		// USERPTR: the caller owns the memory. Each slot is a distinct DSP
		// input buffer, so the VPIF DMA engine round-robins through them while
		// the DSP processes the previous frames.
		for (uint32_t i = 0; i < count; ++i) {
			auto *ptr = static_cast<uint8_t *>(mUserPtrBuffers[static_cast<int>(i)]);
			mMmapBufs.push_back({ptr, mUserPtrSize});
		}
	} else {
		for (uint32_t i = 0; i < count; ++i) {
			v4l2_buffer buf = {};
			buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
			buf.memory = V4L2_MEMORY_MMAP;
			buf.index = i;

			if (ioctl(mFd, VIDIOC_QUERYBUF, &buf) < 0) {
				QLOG_ERROR() << "VideoDeviceFileBase: QUERYBUF" << i << "failed:" << strerror(errno);
				freeBuffers();
				return false;
			}

			QLOG_DEBUG() << "VideoDeviceFileBase: QUERYBUF" << i << "length=" << buf.length
				     << "offset=" << buf.m.offset;

			auto *map = mmap(nullptr, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, mFd, buf.m.offset);
			if (map == MAP_FAILED) {
				QLOG_ERROR() << "VideoDeviceFileBase: mmap" << i << "failed:" << strerror(errno);
				freeBuffers();
				return false;
			}

			mMmapBufs.push_back({static_cast<uint8_t *>(map), buf.length});
		}
	}

	QLOG_INFO() << "VideoDeviceFileBase:" << mMmapBufs.size() << "buffers allocated"
		    << (mUseUserPtr ? "(USERPTR)" : "(MMAP)");
	return true;
}

void VideoDeviceFileBase::freeBuffers()
{
	if (mUseUserPtr) {
		// Caller owns the memory - just drop the pointers.
		mMmapBufs.clear();
	} else {
		for (auto &buf : mMmapBufs) {
			if (buf.data != nullptr && buf.data != MAP_FAILED)
				munmap(buf.data, buf.size);
		}
		mMmapBufs.clear();
	}

	// Release the driver-side buffers too, so a close()/open() cycle (or a
	// partial allocation failure) leaves the device with a clean request.
	if (mFd >= 0) {
		v4l2_requestbuffers req = {};
		req.count = 0;
		req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		req.memory = mUseUserPtr ? V4L2_MEMORY_USERPTR : V4L2_MEMORY_MMAP;
		if (ioctl(mFd, VIDIOC_REQBUFS, &req) < 0) {
			QLOG_WARN() << "VideoDeviceFileBase: REQBUFS(0) failed:" << strerror(errno);
		}
	}
}

bool VideoDeviceFileBase::startStreaming()
{
	// Queue every buffer so the VPIF DMA engine keeps capturing into the free
	// slots while the DSP processes the dequeued ones (triple buffering).
	for (uint32_t i = 0; i < static_cast<uint32_t>(mMmapBufs.size()); ++i) {
		v4l2_buffer buf = {};
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.index = i;

		if (mUseUserPtr) {
			buf.memory = V4L2_MEMORY_USERPTR;
			buf.m.userptr = reinterpret_cast<unsigned long>(mMmapBufs[static_cast<int>(i)].data);
			buf.length = mMmapBufs[static_cast<int>(i)].size;
		} else {
			buf.memory = V4L2_MEMORY_MMAP;
		}

		if (ioctl(mFd, VIDIOC_QBUF, &buf) < 0) {
			QLOG_ERROR() << "VideoDeviceFileBase: QBUF" << i << "failed:" << strerror(errno);
			return false;
		}
	}

	v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(mFd, VIDIOC_STREAMON, &type) < 0) {
		QLOG_ERROR() << "VideoDeviceFileBase: STREAMON failed:" << strerror(errno);
		return false;
	}

	mStreaming = true;

	mNotifier.reset(new QSocketNotifier(mFd, QSocketNotifier::Read, this));
	connect(mNotifier.data(), &QSocketNotifier::activated, this, &VideoDeviceFileBase::onActivated);

	return true;
}

void VideoDeviceFileBase::stopStreaming()
{
	if (!mStreaming || mFd < 0)
		return;

	teardownNotifier();

	v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	if (ioctl(mFd, VIDIOC_STREAMOFF, &type) < 0) {
		QLOG_ERROR() << "VideoDeviceFileBase: STREAMOFF failed:" << strerror(errno);
	}
	mStreaming = false;
}
