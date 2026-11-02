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

#include "trikUsbDevice.h"

#include <QtCore/QByteArray>
#include <QsLog.h>

#include <cstring>
#include <fcntl.h>
#include <linux/nrf-usb.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <unistd.h>

using namespace trikHal;
using namespace trikHal::trik;

TrikUsbDevice::TrikUsbDevice() = default;

TrikUsbDevice::~TrikUsbDevice()
{
	disconnect();
}

void TrikUsbDevice::eventLoop()
{
	uint8_t buf[kTelemetrySize];

	while (mRunning.load(std::memory_order_acquire)) {
		struct pollfd pfd {};
		pfd.fd = mFd;
		pfd.events = POLLIN;

		int ret = poll(&pfd, 1, 1000);
		if (ret < 0) {
			if (errno == EINTR)
				continue;
			break;
		}
		if (ret == 0)
			continue;
		if (!(pfd.revents & POLLIN))
			continue;

		const auto readResult = ::read(mFd, buf, sizeof(buf));
		if (readResult < 0) {
			if (errno == EINTR || errno == EAGAIN)
				continue;
			break;
		}
		if (readResult == 0)
			continue;

		if (mDataCallback) {
			const auto bytesRead = static_cast<int>(readResult);
			QByteArray data(QByteArray::fromRawData(reinterpret_cast<const char *>(buf), bytesRead));
			mDataCallback(data, mDataCallbackUserData);
		}
	}
}

bool TrikUsbDevice::connect()
{
	if (mFd >= 0)
		return true;

	mFd = ::open(kDevicePath, O_RDWR | O_CLOEXEC);
	if (mFd < 0) {
		QLOG_ERROR() << "TrikUsbDevice: open" << kDevicePath << "failed:" << ::strerror(errno);
		return false;
	}

	mRunning.store(true, std::memory_order_release);
	mEventThread = std::thread(&TrikUsbDevice::eventLoop, this);

	QLOG_INFO() << "TrikUsbDevice: connected to" << kDevicePath;
	return true;
}

void TrikUsbDevice::disconnect()
{
	if (!mRunning.exchange(false, std::memory_order_acq_rel))
		return;

	if (mEventThread.joinable())
		mEventThread.join();

	if (mFd >= 0) {
		::close(mFd);
		mFd = -1;
	}

	QLOG_INFO() << "TrikUsbDevice: disconnected";
}

bool TrikUsbDevice::isConnected() const
{
	return mRunning.load(std::memory_order_acquire) && mFd >= 0;
}

bool TrikUsbDevice::sendBulk(const QByteArray &data)
{
	if (mFd < 0)
		return false;

	struct nrf_bulk b {};
	b.data = const_cast<char *>(data.constData());
	b.length = static_cast<uint16_t>(data.size());

	return ::ioctl(mFd, NRF_IO_BULK, &b) >= 0;
}

int TrikUsbDevice::send(const QByteArray &data)
{
	// All outgoing commands go over the same transport; the driver resolves
	// the endpoint, so there is no separate interrupt path.
	return sendBulk(data) ? 0 : -1;
}

void TrikUsbDevice::setDataCallback(DataCallback callback, void *userData)
{
	mDataCallback = callback;
	mDataCallbackUserData = userData;
}

void TrikUsbDevice::setTelemetryPeriodUs(uint32_t periodUs, uint32_t isoPacketsPerUrb)
{
	Q_UNUSED(periodUs)
	Q_UNUSED(isoPacketsPerUrb)
}

bool TrikUsbDevice::controlWrite(uint8_t bRequest, const QByteArray &data, int timeoutMs)
{
	struct nrf_ctrl ctrl = {};
	ctrl.bRequest = bRequest;
	ctrl.bmRequestType = 0x41;
	ctrl.wValue = 0;
	ctrl.wIndex = 0;
	ctrl.wLength = static_cast<uint16_t>(data.size());
	ctrl.timeout = timeoutMs;
	memcpy(ctrl.data, data.data(), static_cast<size_t>(data.size()));

	auto result = ::ioctl(mFd, NRF_IO_CTRL, &ctrl);
	return result >= 0;
}

int TrikUsbDevice::read(const QByteArray &data)
{
	// The nRF path receives telemetry asynchronously via the data callback.
	Q_UNUSED(data)
	return 0;
}