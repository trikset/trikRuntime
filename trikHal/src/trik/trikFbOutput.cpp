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

#include "trikFbOutput.h"

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <unistd.h>

#include <QsLog.h>

using namespace trikHal::trik;

namespace {

/// DSP output geometry: hardcoded 240x240 RGB565 (see the class comment about
/// the temporary direct-to-framebuffer hack).
constexpr uint32_t kFrameWidth = 240;
constexpr uint32_t kFrameHeight = 240;
constexpr size_t kFrameBytes = kFrameWidth * kFrameHeight * 2;

} // namespace

TrikFbOutput::TrikFbOutput(QObject *parent)
	: FbOutputInterface(parent)
{
}

TrikFbOutput::~TrikFbOutput()
{
	close();
}

bool TrikFbOutput::open()
{
	if (mOpen)
		return true;

	mFd = ::open("/dev/fb0", O_RDWR | O_CLOEXEC);
	if (mFd < 0) {
		QLOG_WARN() << "TrikFbOutput: cannot open /dev/fb0:" << strerror(errno);
		return false;
	}

	fb_fix_screeninfo finfo = {};
	if (::ioctl(mFd, FBIOGET_FSCREENINFO, &finfo) < 0) {
		close();
		QLOG_WARN() << "TrikFbOutput: FBIOGET_FSCREENINFO failed:" << strerror(errno);
		return false;
	}

	mMapLen = finfo.smem_len;
	mMap = static_cast<uint8_t *>(::mmap(nullptr, mMapLen, PROT_READ | PROT_WRITE, MAP_SHARED, mFd, 0));
	if (mMap == MAP_FAILED) {
		mMap = nullptr;
		close();
		QLOG_WARN() << "TrikFbOutput: mmap failed:" << strerror(errno);
		return false;
	}

	mOpen = true;
	return true;
}

void TrikFbOutput::close()
{
	if (mMap) {
		::munmap(mMap, mMapLen);
		mMap = nullptr;
	}
	mMapLen = 0;

	if (mFd >= 0) {
		::close(mFd);
		mFd = -1;
	}

	mOpen = false;
}

bool TrikFbOutput::isOpen() const
{
	return mOpen;
}

void TrikFbOutput::writeFrame(const uint8_t *rgb565)
{
	if (!mOpen || !mMap || !rgb565)
		return;

	// The framebuffer is expected to be at least 240x240x2, but guard anyway so
	// a smaller/misconfigured framebuffer cannot be overrun by the memcpy below.
	if (mMapLen < kFrameBytes)
		return;

	::memcpy(mMap, rgb565, kFrameBytes);
}

uint32_t TrikFbOutput::frameWidth() const
{
	return kFrameWidth;
}

uint32_t TrikFbOutput::frameHeight() const
{
	return kFrameHeight;
}
