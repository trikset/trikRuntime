#include "trikV4l2VideoDevice.h"

#include <sys/ioctl.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <libv4l2.h>

#include "QsLog.h"

TrikV4l2VideoDevice::TrikV4l2VideoDevice(const QString &inputFile)
	: fileDevicePath(inputFile)
{
	openDevice();
	setFormat();
}

TrikV4l2VideoDevice::~TrikV4l2VideoDevice()
{
	closeDevice();
}

int TrikV4l2VideoDevice::xioctl(long request, void *arg, const QString &possibleError)
{
	int r = ::v4l2_ioctl (mFileDescriptor, request, arg);

	if (r != 0) {
		if (errno == EAGAIN) {
			QLOG_INFO() << "V4l2 xioctl returned EAGAIN";
			return r;
		}

		QLOG_ERROR() << "ioctl code " << QString("%1").arg(request, 0, 16) << " failed";
		QLOG_ERROR() << errno << ", " << strerror(errno);
		QLOG_ERROR() << possibleError;
	}

	return r;
}

void TrikV4l2VideoDevice::openDevice()
{
	mFileDescriptor = ::v4l2_open(fileDevicePath.toStdString().c_str(), O_RDWR /* required */ | O_NONBLOCK, 0);

	if (mFileDescriptor < 0) {
		QLOG_ERROR() << "Cannot open '" << fileDevicePath << "'";
	} else {
		QLOG_INFO() << "Open v4l2 camera device" <<  fileDevicePath << ",fd =" << mFileDescriptor;
	}

}

void TrikV4l2VideoDevice::setFormat()
{
	mFormat.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
	mFormat.fmt.pix.width = 320;
	mFormat.fmt.pix.height = 240;
	mFormat.fmt.pix.pixelformat = V4L2_PIX_FMT_YUV422P;
	mFormat.fmt.pix.field = V4L2_FIELD_NONE;
	v4l2_std_id stdid = V4L2_STD_625_50;
	if (xioctl(VIDIOC_S_STD, &stdid, "VIDIOC_S_STD in TrikV4l2VideoDevice::setFormat() failed")) {
		if(!xioctl(VIDIOC_G_STD, &stdid, "VIDIOC_G_STD in TrikV4l2VideoDevice::setFormat() failed")) {
			QLOG_INFO() << "VIDIOC_G_STD returned" << QString("%1").arg(stdid, 0, 16);
		}
	}

	if (xioctl (VIDIOC_TRY_FMT, &mFormat, "VIDIOC_TRY_FMT in TrikV4l2VideoDevice::setFormat() failed")) {
		return;
	}
	if (xioctl (VIDIOC_S_FMT, &mFormat, "VIDIOC_S_FMT in TrikV4l2VideoDevice::setFormat() failed")) {
		return;
	}
	// on return, fmt contains actually used image format
	// warn if format is emulated
	
	for (size_t fmtIdx = 0; ; ++fmtIdx) {
		struct v4l2_fmtdesc fmtDesc {};
		fmtDesc.index = fmtIdx;
		fmtDesc.type = mFormat.type;
		if (xioctl(VIDIOC_ENUM_FMT, &fmtDesc, "V4l2 VIDIOC_ENUM_FMT failed: " )) {
			// either fault or unknown format
			break; // just warn, do not fail
		}
	
		if (fmtDesc.pixelformat == mFormat.fmt.pix.pixelformat) {
			if (fmtDesc.flags & V4L2_FMT_FLAG_EMULATED) {
				char fourcc[5] = {0};
				strncpy(fourcc, (char*)&mFormat.fmt.pix.pixelformat, 4);
				QLOG_WARN() << "V4L2 format " << fourcc << "  is emulated, performance will be degraded";
				break;
	  		}
		}
	}
}

void TrikV4l2VideoDevice::closeDevice()
{
	if (mNotifier) {
		mNotifier->setEnabled(false);
		delete mNotifier;
		mNotifier = nullptr;
	}

	if (::v4l2_close(mFileDescriptor)) {
		 QLOG_ERROR() << "Failed to close v4l2 camera device, device path = " << fileDevicePath;
		 return;
	}
	mFileDescriptor = -1;

	QLOG_INFO() << "Close v4l2 camera device " <<  fileDevicePath;
}


QVector<uint8_t> TrikV4l2VideoDevice::makeShot()
{
	initMMAP();

	startCapturing();

	QEventLoop loop;

	connect(this, SIGNAL(dataReady()), &loop, SLOT(quit()));
	loop.exec();
	auto res = getFrame();

	stopCapturing();
	freeMMAP();

	return res;
}

void TrikV4l2VideoDevice::initMMAP()
{
	struct v4l2_requestbuffers req {};

	req.count = 3; // as in sensors
	req.type = mFormat.type;
	req.memory = V4L2_MEMORY_MMAP;
 
	if (xioctl(VIDIOC_REQBUFS, &req, "V4l2 VIDIOC_REQBUFS failed")){
		return;
	}

	QLOG_INFO() << "V4l2 prepared" << req.count << "buffers";

	buffers.resize(req.count);

	for (int i = 0; i < buffers.size(); ++i) {
		struct v4l2_buffer buf {};
		buf.type = mFormat.type;
		buf.index = i;
		buf.memory = V4L2_MEMORY_MMAP;

		if (xioctl(VIDIOC_QUERYBUF, &buf, "V4l2 VIDIOC_QUERYBUF failed")) {
			return;
		}

		buffers[i].length = buf.length;
		buffers[i].start = (uint8_t *) ::v4l2_mmap(NULL, buf.length
							, PROT_READ | PROT_WRITE, MAP_SHARED
							, mFileDescriptor, buf.m.offset);

		if (buffers[i].start == MAP_FAILED) {
			QLOG_ERROR() << "mmap failed in TrikV4l2VideoDevice::initMMAP()";
		}

	}
	QLOG_INFO() << "Init mmap for v4l2 camera device";
}

void TrikV4l2VideoDevice::startCapturing()
{
	for(int i = 0; i < buffers.size(); ++i) {
		struct v4l2_buffer buf {};
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
	mNotifier = new QSocketNotifier(mFileDescriptor, QSocketNotifier::Read, this);
	connect(mNotifier, SIGNAL(activated(int)), this, SLOT(readFrameData(int)));
	connect(mNotifier, &QSocketNotifier::activated, [](){printf("Frame!");});
}

void TrikV4l2VideoDevice::readFrameData(int fd) {
	if (fd != mFileDescriptor) {
		QLOG_ERROR() << "V4l2: readFrame data requested for incorrect fd="<<fd;
		return;
	}


	v4l2_buffer buf {};
	buf.type = mFormat.type;
	buf.memory = V4L2_MEMORY_MMAP;

	//int res = EAGAIN;
	//for(int i = 0; EAGAIN == res && i < 5; res = xioctl(VIDIOC_DQBUF, &buf, "V4l2 VIDIOC_DQBUF failed"),++i) {
	//	QLOG_INFO() << "V4l2: ReadFrame attempt";
	//}

	//if (res) {
	//	return -1;
	//}

	xioctl(VIDIOC_DQBUF, &buf, "V4l2 VIDIOC_DQBUF failed");
	QLOG_INFO() << "V4l2 captured" << buf.bytesused << "bytes into buf #" << buf.index;
	QVector<uint8_t> res;

	if (buf.index < buffers.size()) {
		auto & b = buffers[buf.index];
		if (b.length > 0) {
			res.resize(b.length);
			std::copy(b.start, b.start + b.length, res.begin());
			QLOG_INFO() << "V4l2 captured " << b.length << "bytes into buf #" << buf.index;
		}
		mFrame.swap(res);
		emit dataReady();
	}
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
		if (b.start != MAP_FAILED && ::v4l2_munmap(b.start, b.length)) {
			QLOG_ERROR() << "Free MMAP error in TrikV4l2VideoDevice::freeMMAP() for buffer";
		}
	}
	QLOG_INFO() << "Free MMAP for v4l2 camera";
}
