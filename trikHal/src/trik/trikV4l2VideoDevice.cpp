/* Copyright 2018 Ivan Tyulyandin and CyberTech Labs Ltd.
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

#include "trikV4l2VideoDevice.h"

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <QtCore/QEventLoop>
#include <QtCore/QTimer>
#include "QsLog.h"

#define v4l2_open open
#define v4l2_close close
#define v4l2_mmap mmap
#define v4l2_munmap munmap
#define v4l2_ioctl ioctl

// convertion funtions
namespace {

	inline unsigned char clip255(int x) { return x >= 255 ? 255 : (x <= 0)? 0 : static_cast<uint8_t>(x); }

	QVector<uint8_t> yuyvToRgb(const QVector<uint8_t> &shot, int height, int width) {
		// yuyv (yuv422) convertion to rgb888
		QVector<uint8_t> result(height * width * 3);
		int startIndex = 0;
		for (auto row = 0; row < height; ++ row) {
			for (auto col = 0; col < width; col+=2) {
				// format is y0 cb y1 cr, cb and cr are equal for 2 pixels
				auto y0 = shot[startIndex];
				auto cb = shot[startIndex + 1];
				auto y1 = shot[startIndex + 2];
				auto cr = shot[startIndex + 3];
				startIndex += 4;

				auto resRgb = &result[(row * width + col) * 3];
//				resRgb[0] = clip255(y0 + 1.402 * (cr - 128)); // red
//				resRgb[1] = clip255(y0 - 0.3441 * (cb - 128) - 0.7141 * (cr - 128)); // green
//				resRgb[2] = clip255(y0 + 1.772 * (cb - 128)); // blue

//				resRgb[3] = clip255(y1 + 1.402 * (cr - 128)); // red
//				resRgb[4] = clip255(y1 - 0.3441 * (cb - 128) - 0.7141 * (cr - 128)); // green
//				resRgb[5] = clip255(y1 + 1.772 * (cb - 128)); // blue

				const auto alpha = 180 * (cr - 128) / 128;
				const auto beta  = 45 * (cb - 128) / 128;
				resRgb[0] = clip255(y0 + alpha); // red
				resRgb[1] = clip255(y0 - beta - alpha / 2); // green
				resRgb[2] = clip255(y0 + 5 * beta); // blue

				resRgb[3] = clip255(y1 + alpha); // red
				resRgb[4] = clip255(y1 - beta - alpha / 2); // green
				resRgb[5] = clip255(y1 + 5 * beta); // blue
			}
		}

		return result;
	}

	QVector<uint8_t> yuv422pToRgb(const QVector<uint8_t> &shot, int height, int width) {
		// yuv422p convertion to rgb888
		QVector<uint8_t> result(height * width * 3);
		if ( width <= 0 || height <= 0 ) {
			return result;
		}
		const auto Y = &shot[0];
		const auto UV = &shot[width * height];

		for (auto row = 0; row < height; ++row) {
			for (auto col = 0; col < width; col+=2) {
				auto startIndex = row * width + col;
				int const y1 = Y[startIndex] - 16;
				int const y2 = Y[startIndex+1] - 16;
				int const u  = UV[startIndex] - 128;
				int const v  = UV[startIndex+1] - 128;
				auto _298y1 = 298 * y1;
				auto _298y2 = 298 * y2;
				auto _409v  = 409 * v;
				auto _100u  = -100 * u;
				auto _516u  = 516 * u;
				auto _208v  = -208 * v;
				auto r1 = clip255 ((_298y1 + _516u + 128) >> 8);
				auto g1 = clip255 ((_298y1 + _100u + _208v + 128) >> 8);
				auto b1 = clip255 ((_298y1 + _409v + 128) >> 8);
				auto r2 = clip255 ((_298y2 + _516u + 128) >> 8);
				auto g2 = clip255 ((_298y2 + _100u + _208v + 128) >> 8);
				auto b2 = clip255 ((_298y2 + _409v + 128) >> 8);

				auto rgb = &result[startIndex*3];
				rgb[0] = r1;
				rgb[1] = g1;
				rgb[2] = b1;
				rgb[3] = r2;
				rgb[4] = g2;
				rgb[5] = b2;
			}
		}

		return result;
	}

	QVector<uint8_t> convertToEmpty(const QVector<uint8_t> &shot, int height, int width) {
		Q_UNUSED(shot);
		Q_UNUSED(height);
		Q_UNUSED(width);
		return QVector<uint8_t>();
	}
}

TrikV4l2VideoDevice::TrikV4l2VideoDevice(const QString &inputFile)
	: fileDevicePath(inputFile), mConvertFunc(convertToEmpty)
{
	openDevice();
	setFormat();
}

TrikV4l2VideoDevice::~TrikV4l2VideoDevice()
{
	closeDevice();
}

int TrikV4l2VideoDevice::xioctl(unsigned long request, void *arg, const QString &possibleError)
{
	int r = ::v4l2_ioctl (mFileDescriptor, request, arg);

	if (r != 0) {
		if (errno == EAGAIN) {
			QLOG_INFO() << "V4l2 xioctl returned EAGAIN";
			return r;
		}

		if (request != VIDIOC_ENUM_FMT || errno != EINVAL) {
			QLOG_ERROR() << "ioctl code " << QString("%1").arg(request, 0, 16) << " failed with errno ="
			<< errno << ", " << strerror(errno) << ":" << possibleError;
		}
	}

	return r;
}

void TrikV4l2VideoDevice::openDevice()
{
	mFileDescriptor = ::v4l2_open(fileDevicePath.toStdString().c_str(), O_RDWR | O_NONBLOCK | O_CLOEXEC, 0);

	if (mFileDescriptor < 0) {
		QLOG_ERROR() << "Cannot open '" << fileDevicePath << "', return code is " << mFileDescriptor ;
	} else {
		QLOG_INFO() << "Open v4l2 camera device" <<  fileDevicePath << ",fd =" << mFileDescriptor;
		v4l2_capability cap {};
		unsigned requested = V4L2_CAP_STREAMING | V4L2_CAP_VIDEO_CAPTURE;

		if(!xioctl(VIDIOC_QUERYCAP, &cap, "VIDIOC_QUERYCAP failed")
			&& (((cap.capabilities & V4L2_CAP_DEVICE_CAPS)? cap.device_caps : cap.capabilities) & requested)
			!= requested)
			{
				QLOG_ERROR() << "V4l2: The device does not handle single-planar video capture"
						<< "with streamingdevice_caps = "
						<< QString("%1").arg(cap.device_caps, 0, 16);
				closeDevice();
			}
	}

}

void TrikV4l2VideoDevice::setFormat()
{
	mFormat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	mFormat.fmt.pix.width = 320;
	mFormat.fmt.pix.height = 240;
	mFormat.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV422P; // will be resetted, if inappropriate
	mFormat.fmt.pix.field = V4L2_FIELD_NONE;

	char descPixelFmt[32] = {0}; // 32 - size of v4l2_fmtdesc.description
	__u32 fmtIdx = 0;
	do {
		v4l2_fmtdesc fmtTry {};
		fmtTry.index = fmtIdx;
		fmtTry.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if ( ! xioctl(VIDIOC_ENUM_FMT, &fmtTry, "VIDIOC_ENUM_FMT")) {

			QLOG_INFO() << "V4l2: available format: " << hex << fmtTry.pixelformat;
			mFormat.fmt.pix.pixelformat = fmtTry.pixelformat;
			memcpy(descPixelFmt, fmtTry.description, sizeof(descPixelFmt));

			// have decoding of those formats:
			// V4L2_PIX_FMT_YUV422P
			// V4L2_PIX_FMT_YUYV
			if (fmtTry.pixelformat == V4L2_PIX_FMT_YUV422P) {
				QLOG_INFO() << "V4l2: found format V4L2_PIX_FMT_YUV422P";
				mConvertFunc = yuv422pToRgb;
				break;
			} else if (fmtTry.pixelformat == V4L2_PIX_FMT_YUYV) {
				QLOG_INFO() << "V4l2: found format V4L2_PIX_FMT_YUYV (YUV422)";
				mConvertFunc = yuyvToRgb;
				break;
			}
		}
		++ fmtIdx;
	} while (errno != EINVAL); // EINVAL => end of supported formats

	if (mConvertFunc == convertToEmpty) {
		QLOG_ERROR() << "TRIK Runtime can not convert " << descPixelFmt
				<< " to RGB888, getPhoto will return empty vector";
	} else {
		QLOG_INFO() << "V4l2: setted format " << descPixelFmt;
	}

	v4l2_std_id stdid = V4L2_STD_625_50;
	if (xioctl(VIDIOC_S_STD, &stdid, "VIDIOC_S_STD in TrikV4l2VideoDevice::setFormat()")) {
		if(!xioctl(VIDIOC_G_STD, &stdid, "VIDIOC_G_STD in TrikV4l2VideoDevice::setFormat()")) {
			QLOG_INFO() << "VIDIOC_G_STD returned" << QString("%1").arg(stdid, 0, 16);
		}
	}
	// `v4l2-compliance -d /dev/video2` reports problems for Logitech C920 with old Kernel
	// fail: ../../../v4l-utils-1.10.1/utils/v4l2-compliance/v4l2-test-formats.cpp(662): Video Capture is valid, but no TRY_FMT was implemented
	// test VIDIOC_TRY_FMT: FAIL
	// warn: ../../../v4l-utils-1.10.1/utils/v4l2-compliance/v4l2-test-formats.cpp(933): S_FMT cannot handle an invalid pixelformat.
	// warn: ../../../v4l-utils-1.10.1/utils/v4l2-compliance/v4l2-test-formats.cpp(934): This may or may not be a problem. For more information see:
	// warn: ../../../v4l-utils-1.10.1/utils/v4l2-compliance/v4l2-test-formats.cpp(935): http://www.mail-archive.com/linux-media@vger.kernel.org/msg56550.html
	// fail: ../../../v4l-utils-1.10.1/utils/v4l2-compliance/v4l2-test-formats.cpp(418): expected EINVAL, but got 5 when getting format for buftype 1
	// fail: ../../../v4l-utils-1.10.1/utils/v4l2-compliance/v4l2-test-formats.cpp(952): Video Capture is valid, but no S_FMT was implemented
	// test VIDIOC_S_FMT: FAIL
	if (xioctl (VIDIOC_TRY_FMT, &mFormat, "VIDIOC_TRY_FMT in TrikV4l2VideoDevice::setFormat()")) {
		QLOG_INFO () << "Read error above, but sometimes it is ok to ignore";
	}
	if (xioctl (VIDIOC_S_FMT, &mFormat, "VIDIOC_S_FMT in TrikV4l2VideoDevice::setFormat()")) {
		QLOG_INFO () << "Read error above, but sometimes it is ok to ignore";
	}
}

void TrikV4l2VideoDevice::closeDevice()
{
	mNotifier.reset();
	if (::v4l2_close(mFileDescriptor)) {
		 QLOG_ERROR() << "Failed to close v4l2 camera device descriptor #" << mFileDescriptor
						<< ", device path: " << fileDevicePath;
	}

	mFileDescriptor = -1;
	QLOG_INFO() << "Closed v4l2 camera device " <<  fileDevicePath;
}


const QVector<uint8_t> & TrikV4l2VideoDevice::makeShot()
{
	QEventLoop loop;
	connect(this, &TrikV4l2VideoDevice::dataReady, &loop, &QEventLoop::quit, Qt::QueuedConnection);
	initMMAP();
	startCapturing();
	QTimer::singleShot(1000, &loop, [&loop](){loop.exit(-1);});
	if (loop.exec() < 0) {
		QLOG_WARN() << "V4l2 makeShot terminated by watchdog timer. Device not ready or misconfigured?";
	}

	stopCapturing();
	freeMMAP();

	constexpr auto IMAGE_WIDTH = 320;
	constexpr auto IMAGE_HEIGHT = 240;
	// expect yuv format, 4 bytes for 2 pixels
	if (mFrame.size() != IMAGE_HEIGHT * IMAGE_WIDTH * 2) {
		QLOG_ERROR() << "V4l2: unexpected size of obtained image (" << mFrame.size() << "instead of"
					 << IMAGE_HEIGHT * IMAGE_WIDTH * 2  << ")";
		QVector<uint8_t>().swap(mFrame);
	} else {
		mConvertFunc(mFrame, IMAGE_HEIGHT, IMAGE_WIDTH).swap(mFrame);
	}
	return mFrame;
}

void TrikV4l2VideoDevice::initMMAP()
{
	v4l2_requestbuffers req{};
	req.count = 1;
	req.type = mFormat.type;
	req.memory = V4L2_MEMORY_MMAP;

	if (xioctl(VIDIOC_REQBUFS, &req, "V4l2 VIDIOC_REQBUFS failed")){
		return;
	}

	QLOG_INFO() << "V4l2 prepared" << req.count << "buffers";

	buffers.resize(req.count);

	for (int i = 0; i < buffers.size(); ++i) {
		v4l2_buffer buf{};
		buf.type = mFormat.type;
		buf.index = i;
		buf.memory = V4L2_MEMORY_MMAP;

		if (xioctl(VIDIOC_QUERYBUF, &buf, "V4l2 VIDIOC_QUERYBUF failed")) {
			return;
		}

		buffers[i].length = buf.length;
		buffers[i].start = static_cast<uint8_t *>(::v4l2_mmap(nullptr, buf.length
							, PROT_READ | PROT_WRITE, MAP_SHARED
							, mFileDescriptor, buf.m.offset));

		if (buffers[i].start == MAP_FAILED) {
			QLOG_ERROR() << "mmap failed in TrikV4l2VideoDevice::initMMAP()";
		}

	}

	QLOG_INFO() << "Init mmap for v4l2 camera device";
}

void TrikV4l2VideoDevice::startCapturing()
{
	mNotifier.reset(new QSocketNotifier(mFileDescriptor, QSocketNotifier::Read));
	connect(&*mNotifier, &QSocketNotifier::activated, this, &TrikV4l2VideoDevice::readFrameData);

	for(int i = 0; i < buffers.size(); ++i) {
		v4l2_buffer buf {};
		buf.type = mFormat.type;
		buf.memory = V4L2_MEMORY_MMAP;
		buf.index = i;

		if (xioctl(VIDIOC_QBUF, &buf, "V4l2 VIDIOC_QBUF failed")) {
			continue;//???
		}
	}

	v4l2_buf_type type {static_cast<v4l2_buf_type> (mFormat.type)};

	if (xioctl(VIDIOC_STREAMON, &type, "V4l2 VIDIOC_STREAMON failed")) {
		return;
	}

	QLOG_INFO() << "V4l2 camera: start capturing";
}

void TrikV4l2VideoDevice::readFrameData(int fd) {
	if (fd != mFileDescriptor) {
		QLOG_ERROR() << "V4l2: readFrame data requested for incorrect fd=" << fd;
		return;
	}

	mNotifier->setEnabled(false);
	v4l2_buffer buf{};
	buf.type = mFormat.type;
	buf.memory = V4L2_MEMORY_MMAP;

	xioctl(VIDIOC_DQBUF, &buf, "V4l2 VIDIOC_DQBUF failed");

	if (buf.index < static_cast<decltype(buf.index)>(buffers.size())) {
		auto &b = buffers[buf.index];
		if (buf.bytesused > 0) {
			mFrame.resize(buf.bytesused);
			std::copy(b.start, b.start + buf.bytesused, mFrame.begin());
			QLOG_DEBUG() << "V4l2 captured " << mFrame.size() << "bytes into buf #" << buf.index;
		} else {
			QLOG_INFO() << "V4l2 captured 0 bytes, reset internal frame buffer";
			decltype(mFrame)().swap(mFrame);
		}
		emit dataReady();
	}
	// WARN: Do not enable notifier until we store process data from mFrame variable!
}

void TrikV4l2VideoDevice::stopCapturing()
{
	v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (!xioctl(VIDIOC_STREAMOFF, &type, "V4l2 VIDIOC_STREAMOFF failed")) {
		QLOG_INFO() << "V4l2 camera: stopped capturing";
	}
}


void TrikV4l2VideoDevice::freeMMAP()
{
	for(auto &b : buffers) {
		if (b.start != MAP_FAILED && ::v4l2_munmap(b.start, b.length)) {
			QLOG_ERROR() << "Free MMAP error in TrikV4l2VideoDevice::freeMMAP() for buffer";
		}
	}

	QLOG_INFO() << "Free MMAP buffers for v4l2 finished";
}
