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

#include "trikVideoDevice.h"

#include <cerrno>
#include <linux/videodev2.h>
#include <sys/ioctl.h>

#include <QsLog.h>

using namespace trikHal::trik;

TrikVideoDevice::TrikVideoDevice(const QString &devicePath,
                                 uint32_t width, uint32_t height, uint32_t fourcc,
                                 uint32_t bufferCount, bool isWebcam)
	: trikHal::VideoDeviceFileBase(devicePath, width, height, fourcc,
	                               bufferCount, isWebcam)
{
}

bool TrikVideoDevice::setFormat()
{
	if (!trikHal::VideoDeviceFileBase::setFormat()) {
		return false;
	}

	// TODO: the analog ov7670 ports deliberately do not set the PAL standard
	// (VIDIOC_S_STD V4L2_STD_625_50) here. The sensor is brought up by the
	// kernel driver's reinit sysfs node / I2C register programming in
	// initVideoSensor(), and the target firmware does not require the legacy
	// `v4l2-ctl -s pal` step. Revisit only if the analog camera ever reports
	// wrong timings.

	for (uint32_t i = 0;; ++i) {
		v4l2_fmtdesc fdesc = {};
		fdesc.index = i;
		fdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
		if (ioctl(mFd, VIDIOC_ENUM_FMT, &fdesc) < 0)
			break;
		if (fdesc.pixelformat == mActualFourcc) {
			if (fdesc.flags & V4L2_FMT_FLAG_EMULATED) {
				QLOG_WARN() << "TrikVideoDevice:" << mPath
				            << "format is emulated, performance will be degraded";
			}
			break;
		}
	}

	return true;
}
