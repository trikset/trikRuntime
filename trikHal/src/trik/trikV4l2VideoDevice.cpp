#include "trikV4l2VideoDevice.h"

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include <linux/videodev2.h>

#include "QsLog.h"

TrikV4l2VideoDevice::TrikV4l2VideoDevice(const QString &inputFile)
	: fd(-1)
	, fileDevicePath(inputFile)
{
	openDevice();
	setFormat();
}

TrikV4l2VideoDevice::~TrikV4l2VideoDevice()
{
	closeDevice();
}

int TrikV4l2VideoDevice::xioctl(int request, void *arg, const QString &possibleError)
{
	int r = ioctl (fd, request, arg);

	if (r != 0) {
		if (errno == EAGAIN) {
			QLOG_INFO() << "V4l2 xioctl returned EAGAIN";
			return EAGAIN;
		}

		QLOG_ERROR() << "ioctl code " << QString("%1").arg(request, 0, 16) << " failed";
		QLOG_ERROR() << errno << ", " << strerror(errno);
		QLOG_ERROR() << possibleError;
	}

	return r;
}

void TrikV4l2VideoDevice::openDevice()
{
	fd = open(fileDevicePath.toStdString().c_str(), O_RDWR /* required */ | O_NONBLOCK, 0);

	if (fd < 0) {
		QLOG_ERROR() << "Cannot open '" << fileDevicePath << "'";
		return;
	}

	QLOG_INFO() << "Open v4l2 camera device" <<  fileDevicePath << ",fd =" << fd;
}

void TrikV4l2VideoDevice::setFormat()
{
	struct v4l2_format fmt = {};
	fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	fmt.fmt.pix.width = 320;
	fmt.fmt.pix.height = 240;
	fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV422P;
	fmt.fmt.pix.field = V4L2_FIELD_NONE;
	v4l2_std_id stdid = V4L2_STD_625_50;
	if (xioctl(VIDIOC_S_STD, &stdid, "VIDIOC_S_STD in TrikV4l2VideoDevice::setFormat() failed")) {
		if(!xioctl(VIDIOC_G_STD, &stdid, "VIDIOC_G_STD in TrikV4l2VideoDevice::setFormat() failed")) {
			QLOG_INFO() << "VIDIOC_G_STD returned" << QString("%1").arg(stdid, 0, 16);
		}
	}

	if (xioctl (VIDIOC_TRY_FMT, &fmt, "VIDIOC_TRY_FMT in TrikV4l2VideoDevice::setFormat() failed")) {
		return;
	}
	if (xioctl (VIDIOC_S_FMT, &fmt, "VIDIOC_S_FMT in TrikV4l2VideoDevice::setFormat() failed")) {
		return;
	}
}

void TrikV4l2VideoDevice::closeDevice()
{
	if (close(fd)) {
		 QLOG_ERROR() << "Failed to close v4l2 camera device, device path = " << fileDevicePath;
		 return;
	}
	fd = -1;

	QLOG_INFO() << "Close v4l2 camera device " <<  fileDevicePath;
}


QVector<uint8_t> TrikV4l2VideoDevice::getFrame()
{
	initMMAP();

	startCapturing();

	int bufNum = readFrame();

	for (long i = 0; i < 10 && ( bufNum < 0 || buffers[bufNum].length == 0);++i) {
		bufNum = readFrame();
	}

	QVector<uint8_t> res;

	if (bufNum >= 0) {
		auto & b = buffers[bufNum];
		if (b.length > 0) {
			res.resize(b.length);
			std::copy(b.start, b.start + b.length, res.begin());
			printf("SHIT %d %d\n", bufNum, b.length);
			QLOG_INFO() << "V4l2 captured " << b.length << "bytes into buf #" << bufNum;
		}
	}

	stopCapturing();

	freeMMAP();

	return res;
}

void TrikV4l2VideoDevice::initMMAP()
{
	struct v4l2_requestbuffers req = {};

	req.count = 4;
	req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	req.memory = V4L2_MEMORY_MMAP;
 
	if (xioctl(VIDIOC_REQBUFS, &req, "V4l2 VIDIOC_REQBUFS failed")){
		return;
	}

	QLOG_INFO() << "V4l2 prepared" << req.count << "buffers";

	buffers.resize(req.count);

	for (size_t i = 0; i < buffers.size(); ++i) {
		struct v4l2_buffer buf = {};
		buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		buf.index = 0;
		buf.memory = V4L2_MEMORY_MMAP;

		if (xioctl(VIDIOC_QUERYBUF, &buf, "V4l2 VIDIOC_QUERYBUF failed")) {
			return;
		}

		buffers[i].length = buf.length;
		buffers[i].start = (uint8_t *) mmap(NULL, buf.length
							, PROT_READ | PROT_WRITE, MAP_SHARED
							, fd, buf.m.offset);

		if (buffers[i].start == MAP_FAILED) {
			QLOG_ERROR() << "mmap failed in TrikV4l2VideoDevice::initMMAP()";
		}

	}
	QLOG_INFO() << "Init mmap for v4l2 camera device";
}

void TrikV4l2VideoDevice::startCapturing()
{
	struct v4l2_buffer buf = {};
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;
	buf.index = 0;

	if (xioctl(VIDIOC_QBUF, &buf, "V4l2 VIDIOC_QBUF failed")) {
		return;
	}

	enum v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (xioctl(VIDIOC_STREAMON, &type, "V4l2 VIDIOC_STREAMON failed")) {
		return;
	}

	QLOG_INFO() << "V4l2 camera: start capturing";
}

int TrikV4l2VideoDevice::readFrame()
{
	v4l2_buffer buf = {};
	buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	buf.memory = V4L2_MEMORY_MMAP;

	int res = EAGAIN;
	for(; EAGAIN == res; res = xioctl(VIDIOC_DQBUF, &buf, "V4l2 VIDIOC_DQBUF failed")) {
	}

	if (res) {
		return -1;
	}

	QLOG_INFO() << "V4l2 captured" << buf.bytesused << "bytes into buf #" << buf.index;
	return buf.index;
}

void TrikV4l2VideoDevice::stopCapturing()
{
	v4l2_buf_type type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

	if (xioctl(VIDIOC_STREAMOFF, &type, "V4l2 VIDIOC_STREAMOFF failed")) {
		return;
	}

	QLOG_INFO() << "V4l2 camera: stop capturing";
}


void TrikV4l2VideoDevice::freeMMAP()
{
	for(auto & b : buffers) { 
		if (b.start != MAP_FAILED && munmap(b.start, b.length)) {
			QLOG_ERROR() << "Free MMAP error in TrikV4l2VideoDevice::freeMMAP() for buffer";
		}
	}
	QLOG_INFO() << "Free MMAP for v4l2 camera";
}
