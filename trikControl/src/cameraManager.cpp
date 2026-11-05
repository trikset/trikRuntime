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

#include "cameraManager.h"

#include <QtCore/QMetaObject>
#include <QtCore/QThread>
#include <QtCore/QVector>

#include <trikHal/VideoDeviceFileInterface.h>
#include <trikHal/hardwareAbstractionInterface.h>
#include <trikKernel/configurer.h>
#include <trikKernel/videoUtils.h>
#include <QsLog.h>

#include <algorithm>
#include <chrono>
#include <cstdio>
#include <memory>
#include <utility>

#ifdef Q_OS_LINUX
#	include <execinfo.h>
#	include <unistd.h>
#endif

#include "configurerHelper.h"
#include "deviceState.h"

namespace trikControl {

void Frame::release() noexcept
{
	if (mToken) {
		if (mToken->refs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
			auto *owner = mOwner.get();
			if (owner && mToken->entryId >= 0) {
				QMetaObject::invokeMethod(owner,
					[owner, id = mToken->entryId, idx = mToken->bufferIdx]() {
					owner->returnBufferToDriverById(id, idx);
				}, Qt::QueuedConnection);
			}
		}
		mToken = nullptr;
	}
	mOwner.reset();
}

CameraManager::CameraManager(const trikKernel::Configurer &configurer, const trikHal::HardwareAbstractionInterface &hal,
	QObject *parent)
	: QObject(parent)
	, mHal(hal)
{

	// Single pass over the config: build the static per-port state. The
	// Configurer is never touched after this loop - all later lookups go
	// through mPortId + mEntries.
	for (auto &&port : configurer.ports()) {
		if (configurer.deviceClass(port) != QStringLiteral("videoDevice")) {
			continue;
		}

		mPortId.insert(port, static_cast<int>(mEntries.size()));
		mEntries.push_back(makeEntry(configurer, port));

		const auto &entry = mEntries.back();
		QLOG_INFO() << "CameraManager: registered port" << port << "->" << entry.devFile << entry.w << 'x'
			    << entry.h << "ready=" << entry.ready;
	}

	// Run the manager (and its V4L2 devices / QSocketNotifiers) on a dedicated
	// thread, so the device I/O never blocks the GUI thread.
	mThread.reset(new QThread);
	mThread->setObjectName(QStringLiteral("CameraManager"));
	moveToThread(mThread.data());
	mThread->start();
}

CameraManager::~CameraManager()
{
	// Tear the devices down on their own thread (blocking), then stop it.
	runInManagerThread([this]() {
		for (std::size_t id = 0; id < mEntries.size(); ++id) {
			tearDownPort(static_cast<int>(id));
		}
	});

	mThread->quit();
	mThread->wait();
}

CameraManager::Entry CameraManager::makeEntry(const trikKernel::Configurer &configurer, const QString &port)
{
	Entry entry;
	DeviceState state("videoDevice");

	entry.portName = port;
	entry.w = static_cast<uint32_t>(ConfigurerHelper::configureInt(configurer, state, port, "width"));
	entry.h = static_cast<uint32_t>(ConfigurerHelper::configureInt(configurer, state, port, "height"));

	QString defaultDevFile, fmtStr, defaultStreamerScript;
	const auto devFile = configurer.attributeByPort(port, "device", &defaultDevFile);
	const auto fmt = configurer.attributeByPort(port, "format", &fmtStr);
	const auto streamerScript = configurer.attributeByPort(port, "mjpgStreamerScript", &defaultStreamerScript);

	entry.devFile = devFile;
	entry.mjpgStreamerScript = streamerScript;
	entry.tokens = std::make_unique<Frame::Token[]>(captureBufferCount);
	entry.ready = !state.isFailed() && !devFile.isEmpty() && !fmt.isEmpty();
	if (entry.ready) {
		entry.fmt = static_cast<uint32_t>(trikKernel::toV4l2Fourcc(trikKernel::pixelFormatFromString(fmt)));
	} else {
		QLOG_WARN() << "CameraManager: port" << port << "has an invalid config, marked not ready";
	}

	return entry;
}



bool CameraManager::openDeviceLocked(int id, Entry &entry)
{
	if (entry.dev) {
		return true;
	}

	const bool isWebcam = entry.portName.startsWith(QStringLiteral("usb-camera"));
	std::unique_ptr<trikHal::VideoDeviceFileInterface> dev(
		mHal.createVideoDeviceFile(entry.devFile, entry.w, entry.h, entry.fmt, isWebcam));

	if (!dev->open()) {
		QLOG_ERROR() << "CameraManager: failed to open" << entry.devFile;
		return false;
	}

	// Cache the actual negotiated format and bytes-per-line for the DSP.
	entry.negotiatedFourcc = dev->actualFourcc();
	entry.lineLength = dev->bytesPerLine();
	entry.negotiatedWidth = dev->actualWidth();
	entry.negotiatedHeight = dev->actualHeight();

	QLOG_DEBUG() << "CameraManager: port" << entry.portName << "opened, actualFourcc=0x" << Qt::hex
		     << dev->actualFourcc() << Qt::dec << "lineLength" << entry.lineLength;

	// Relay frames from the device to the pull slot (both live on the manager
	// thread, so this connection is direct).
	connect(dev.get(), &trikHal::VideoDeviceFileInterface::frameReady, this,
		[this, id](uint32_t bufferIdx, const uint8_t *data, size_t size) {
		onDeviceFrame(id, bufferIdx, data, size);
	});

	entry.dev = std::move(dev);
	return true;
}

void CameraManager::onDeviceFrame(int id, uint32_t bufferIdx, const uint8_t *data, size_t size)
{
#ifdef TRIK_DEBUG_FPS
	mCaptureFps.tick();
#endif
	auto &e = mEntries[id];
	if (!e.streaming || !e.dev) {
		return;
	}

	const uint32_t seq = ++e.captureSeq;
	auto &t = e.tokens[bufferIdx];
	t.captured = true;
	t.entryId = id;
	t.bufferIdx = bufferIdx;
	t.seq = seq;
	t.data = data;
	t.size = size;
	t.refs.store(1, std::memory_order_relaxed);

	Frame::Token *old = nullptr;
	{
		std::lock_guard<std::mutex> lock(mFrameLock);
		old = e.latest;
		e.latest = &t;
		mFrameCond.notify_all();
	}
	if (old && old->refs.fetch_sub(1, std::memory_order_acq_rel) == 1) {
		returnBufferToDriver(e, old->bufferIdx);
		old->captured = false;
	}
}

void CameraManager::returnBufferToDriver(Entry &entry, uint32_t bufferIdx)
{
	if (entry.dev) {
		entry.dev->release(bufferIdx);
	}
}

void CameraManager::returnBufferToDriverById(int id, uint32_t bufferIdx)
{
	if (id < 0 || static_cast<std::size_t>(id) >= mEntries.size()) {
		return;
	}
	auto &e = mEntries[id];
	if (e.dev) {
		e.dev->release(bufferIdx);
	}
}

void CameraManager::dropLatest(Entry &entry)
{
	std::lock_guard<std::mutex> lock(mFrameLock);
	if (entry.latest) {
		entry.latest->refs.fetch_sub(1, std::memory_order_acq_rel);
		entry.latest = nullptr;
	}
	// STREAMOFF already returned every buffer to the driver; forget the
	// per-buffer "captured" marks, otherwise the first frame after a restart
	// would double-queue a buffer that is already in the driver's queue.
	for (uint32_t i = 0; i < captureBufferCount; ++i) {
		entry.tokens[i].captured = false;
	}
}

Frame CameraManager::getFrame(const QString &port, uint32_t afterSeq, int timeoutMs, const std::atomic<bool> *abort)
{
	return getFrame(mPortId.value(port, -1), afterSeq, timeoutMs, abort);
}

Frame CameraManager::getFrame(int portId, uint32_t afterSeq, int timeoutMs, const std::atomic<bool> *abort)
{
	if (portId < 0 || static_cast<std::size_t>(portId) >= mEntries.size()) {
		return {};
	}
	auto &e = mEntries[portId];

	const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(std::max(timeoutMs, 0));

	std::unique_lock<std::mutex> lock(mFrameLock);
	for (;;) {
		if (abort && abort->load(std::memory_order_acquire)) {
			return {};
		}
		if (e.latest && e.latest->seq > afterSeq) {
			// Relaxed: the token fields are already ordered by mFrameLock (the
			// manager writes them before its locked `e.latest` assignment, we
			// read `e.latest` under the same lock); the add only claims the
			// frame. The release side (Frame::release) still uses acq_rel so the
			// buffer reads happen before the refcount drop.
			e.latest->refs.fetch_add(1, std::memory_order_relaxed); // the client's claim
			return {sharedFromThis(), e.latest};
		}
		if (timeoutMs == 0) {
			return {};
		}
		if (timeoutMs < 0) {
			mFrameCond.wait_for(lock, std::chrono::seconds(1), [&] {
				return (abort && abort->load(std::memory_order_acquire))
				       || (e.latest && e.latest->seq > afterSeq);
			});
			continue;
		}
		const auto remaining = std::chrono::duration_cast<std::chrono::milliseconds>(
			deadline - std::chrono::steady_clock::now());
		if (remaining.count() <= 0) {
			return {};
		}
		mFrameCond.wait_for(lock, remaining);
	}
}

void CameraManager::abortFrameWaits(std::atomic<bool> *flag)
{
	// Store the abort flag and notify under mFrameLock. A waiter blocked in
	// getFrame() holds this lock only while checking its predicate and releases
	// it atomically when it blocks, so the notify below can never fire between
	// a predicate check and the actual sleep - no lost wakeup, no polling slice.
	std::lock_guard<std::mutex> lock(mFrameLock);
	if (flag) {
		flag->store(true, std::memory_order_release);
	}
	mFrameCond.notify_all();
}

void CameraManager::acquire(const QString &port)
{
	acquire(mPortId.value(port, -1));
}

void CameraManager::acquire(int portId)
{
	runAsync([this, portId]() {
		if (portId < 0 || static_cast<std::size_t>(portId) >= mEntries.size()) {
			QLOG_ERROR() << "CameraManager: unknown port id" << portId;
			return;
		}
		auto &e = mEntries[portId];
		if (!e.ready) {
			QLOG_ERROR() << "CameraManager: port" << e.portName << "has an invalid config";
			return;
		}
		if (!e.dev && !openDeviceLocked(portId, e)) {
			QLOG_ERROR() << "CameraManager: failed to open" << e.devFile;
			return;
		}
		if (!e.streaming && !e.dev->startStreaming()) {
			QLOG_ERROR() << "CameraManager: failed to start streaming on" << e.devFile;
			return;
		}
		e.streaming = true;
		++e.refCount;
		QLOG_INFO() << "CameraManager: acquired" << e.portName << "refs=" << e.refCount;
	});
}

void CameraManager::release(const QString &port)
{
	release(mPortId.value(port, -1));
}

void CameraManager::release(int portId)
{
	runAsync([this, portId]() {
		if (portId < 0 || static_cast<std::size_t>(portId) >= mEntries.size() || !mEntries[portId].ready) {
			return;
		}
		auto &e = mEntries[portId];
		if (e.refCount <= 0) {
			return;
		}
		if (--e.refCount > 0) {
			return;
		}

		// Last client gone: stop the stream (STREAMOFF returns every buffer).
		if (e.streaming) {
			e.dev->stopStreaming();
			e.streaming = false;
			dropLatest(e);
		}

		// A latched stop parks the camera (kept open for a quick re-acquire);
		// otherwise the device is torn down for good.
		if (!e.stopLatched) {
			e.dev->close();
			e.dev.reset();
			QLOG_INFO() << "CameraManager: released" << e.portName << "(closed)";
		} else {
			QLOG_INFO() << "CameraManager: released" << e.portName << "(parked)";
		}
	});
}

void CameraManager::stopStreaming(const QString &port)
{
	runAsync([this, port]() {
		const int id = mPortId.value(port, -1);
		if (id < 0 || !mEntries[id].dev) {
			return;
		}
		auto &e = mEntries[id];
		// Latch the park request. If the requester is the only client (or no
		// one else is holding) park right away; otherwise it happens when the
		// last client releases. The latch survives later acquire()/release()
		// cycles, so the camera parks again after the next client lets go.
		e.stopLatched = true;
		if (e.refCount <= 1 && e.streaming) {
			e.dev->stopStreaming();
			e.streaming = false;
			dropLatest(e);
		}
		QLOG_INFO() << "CameraManager: stopStreaming latched on" << port;
	});
}

void CameraManager::close(const QString &port)
{
	QLOG_INFO() << "CameraManager: close on port" << port;
	runInManagerThread([this, port]() {
		const int id = mPortId.value(port, -1);
		if (id >= 0) {
			tearDownPort(id);
		}
	});
}

void CameraManager::close()
{
	QLOG_INFO() << "CameraManager: close all cameras";
	runInManagerThread([this]() {
		for (std::size_t id = 0; id < mEntries.size(); ++id) {
			tearDownPort(static_cast<int>(id));
		}
	});
}

void CameraManager::tearDownPort(int id)
{
	auto &e = mEntries[id];
	dropLatest(e);
	if (e.dev) {
		e.dev->disconnect();
		e.dev->stopStreaming();
		e.dev->close();
		e.dev.reset();
	}
	e.streaming = false;
	e.refCount = 0;
	e.stopLatched = false;
}

int CameraManager::portId(const QString &port) const
{
	return mPortId.value(port, -1);
}

QString CameraManager::portName(int portId) const
{
	if (portId < 0 || static_cast<std::size_t>(portId) >= mEntries.size()) {
		return {};
	}
	return mEntries[portId].portName;
}

uint32_t CameraManager::width(const QString &port) const
{
	const int id = mPortId.value(port, -1);
	return (id >= 0 && mEntries[id].ready) ? mEntries[id].w : 0;
}

uint32_t CameraManager::height(const QString &port) const
{
	const int id = mPortId.value(port, -1);
	return (id >= 0 && mEntries[id].ready) ? mEntries[id].h : 0;
}

uint32_t CameraManager::fourcc(const QString &port) const
{
	const int id = mPortId.value(port, -1);
	return (id >= 0 && mEntries[id].ready) ? mEntries[id].fmt : 0;
}

trikKernel::PixelFormat CameraManager::format(const QString &port) const
{
	const int id = mPortId.value(port, -1);
	return (id >= 0 && mEntries[id].ready) ? trikKernel::fromV4l2Fourcc(mEntries[id].negotiatedFourcc)
	                                       : trikKernel::PixelFormat::Unknown;
}

uint32_t CameraManager::lineLength(const QString &port) const
{
	const int id = mPortId.value(port, -1);
	return (id >= 0 && mEntries[id].ready) ? mEntries[id].lineLength : 0;
}

QString CameraManager::deviceFile(const QString &port) const
{
	const int id = mPortId.value(port, -1);
	return (id >= 0 && mEntries[id].ready) ? mEntries[id].devFile : QString();
}

QString CameraManager::streamerScript(const QString &port) const
{
	const int id = mPortId.value(port, -1);
	return id >= 0 ? mEntries[id].mjpgStreamerScript : QString();
}

bool CameraManager::PortInfo::captureGeometry(size_t frameBytes, uint32_t *width, uint32_t *height,
	uint32_t *stride) const
{
	const uint32_t w = actualWidth > 0 ? actualWidth : this->width;
	const uint32_t h = actualHeight > 0 ? actualHeight : this->height;
	if (w == 0 || h == 0 || format == trikKernel::PixelFormat::Unknown) {
		return false;
	}

	auto packedStride = [this](uint32_t width) {
		return format == trikKernel::PixelFormat::Yuyv ? width * 2 : width;
	};
	auto byteSize = [this, h](uint32_t s) {
		return (format == trikKernel::PixelFormat::Nv16 ? 2 : 1) * static_cast<size_t>(s) * h;
	};

	uint32_t s = lineLength > 0 ? lineLength : packedStride(w);
	if (frameBytes < byteSize(s)) {
		s = packedStride(w);
		if (frameBytes < byteSize(s)) {
			return false;
		}
	}

	if (width) {
		*width = w;
	}
	if (height) {
		*height = h;
	}
	if (stride) {
		*stride = s;
	}
	return true;
}

CameraManager::PortInfo CameraManager::info(const QString &port) const
{
	return info(mPortId.value(port, -1));
}

CameraManager::PortInfo CameraManager::info(int portId) const
{
	PortInfo result;
	if (portId < 0 || static_cast<std::size_t>(portId) >= mEntries.size() || !mEntries[portId].ready) {
		return result;
	}
	const auto &e = mEntries[portId];
	result.width = e.w;
	result.height = e.h;
	result.actualWidth = e.negotiatedWidth;
	result.actualHeight = e.negotiatedHeight;
	result.fourcc = e.fmt;
	result.format = trikKernel::fromV4l2Fourcc(e.negotiatedFourcc);
	result.lineLength = e.lineLength;
	result.inputRegion = 0;
	result.inputBuffersPerRegion = captureBufferCount;
	return result;
}

} // namespace trikControl
