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
#include <trikDsp/dspTypes.h>
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
				QMetaObject::invokeMethod(owner, [owner,
					id = mToken->entryId, idx = mToken->bufferIdx]() {
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
	auto nextRegion = 0u;
	for (auto &&port : configurer.ports()) {
		if (configurer.deviceClass(port) != QStringLiteral("videoDevice")) {
			continue;
		}

		const auto region = nextRegion;
		nextRegion = (nextRegion + 1) % trikKernel::dspInputRegions;

		mPortId.insert(port, static_cast<int>(mEntries.size()));
		mEntries.push_back(makeEntry(configurer, port, region));

		const auto &entry = mEntries.back();
		QLOG_INFO() << "CameraManager: registered port" << port << "->" << entry.devFile << entry.w << 'x'
			    << entry.h << "ready=" << entry.ready << "region=" << region;
	}

#ifdef Q_OS_LINUX
	// Map the capture region up front (before the DSP's INIT response): the
	// manager hands the per-port virtual addresses to V4L2 as USERPTR targets.
	if (mapInputRegion()) {
		QLOG_INFO() << "CameraManager: capture region mapped, zero-copy streaming available";
	} else {
		QLOG_WARN() << "CameraManager: capture region unavailable, USERPTR disabled (MMAP fallback)";
	}
#endif

	// Run the manager (and its V4L2 devices / QSocketNotifiers) on a dedicated
	// thread, so the slow sensor initialization and device I/O never block the
	// GUI thread.
	mThread.reset(new QThread);
	mThread->setObjectName(QStringLiteral("CameraManager"));
	moveToThread(mThread.data());
	mThread->start();

	// Kick off the (possibly slow) ov7670 initialization on the worker thread,
	// so the Brick constructor never blocks.
	runAsync([this]() { initSensors(); });
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
	// mInputMap munmaps itself (RAII).
}

CameraManager::Entry CameraManager::makeEntry(const trikKernel::Configurer &configurer, const QString &port,
	uint32_t region)
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

	// The ov7670 analog ports (named "video*") carry the I2C bus/address and
	// reset GPIO used for sensor initialization. Only those ports have them.
	if (port.startsWith(QStringLiteral("video"))) {
		const auto optInt = [&configurer, &port](const QString &name) {
			try {
				bool ok = false;
				const int v = configurer.attributeByPort(port, name).toInt(&ok, 0);
				return ok ? v : 0;
			} catch (const trikKernel::MalformedConfigException &) {
				return 0;
			}
		};
		entry.i2cBus = optInt(QStringLiteral("i2cBus"));
		entry.i2cAddress = optInt(QStringLiteral("i2cAddress"));
		entry.gpioNumber = optInt(QStringLiteral("gpioNumber"));
	}

	entry.devFile = devFile;
	entry.mjpgStreamerScript = streamerScript;
	entry.inputRegion = region;
	entry.tokens = std::make_unique<Frame::Token[]>(inputBuffersPerRegionValue);
	entry.ready = !state.isFailed() && !devFile.isEmpty() && !fmt.isEmpty();
	if (entry.ready) {
		entry.fmt = static_cast<uint32_t>(trikKernel::toV4l2Fourcc(trikDsp::pixelFormatFromString(fmt)));
	} else {
		QLOG_WARN() << "CameraManager: port" << port << "has an invalid config, marked not ready";
	}

	return entry;
}

#ifdef Q_OS_LINUX
bool CameraManager::mapInputRegion()
{
	const auto regionLen = trikKernel::dspInputBufferTotal * trikKernel::dspInputFrameSize;
	mInputMap = trikHal::mapPhysicalMemory(trikKernel::dspInputPhysAddress, regionLen);
	if (!mInputMap) {
		QLOG_WARN() << "CameraManager: failed to map capture region at 0x" << Qt::hex
			    << trikKernel::dspInputPhysAddress;
		return false;
	}

	QLOG_INFO() << "CameraManager: mapped capture region at" << static_cast<void *>(mInputMap.data()) << "size"
		    << regionLen;
	return true;
}
#endif // Q_OS_LINUX

bool CameraManager::openDeviceLocked(int id, Entry &entry)
{
	if (entry.dev) {
		return true;
	}

	// The analog ov7670 cameras (ports named "video*") are initialized over
	// I2C; the USB webcam is not. Done only once per process lifetime.
	const bool isVideoPort = entry.portName.startsWith(QStringLiteral("video"));
	if (isVideoPort && !entry.sensorInitialized) {
		entry.sensorInitialized =
			mHal.initVideoSensor(entry.devFile, entry.i2cBus, entry.i2cAddress, entry.gpioNumber);
	}

	std::unique_ptr<trikHal::VideoDeviceFileInterface> dev(
		mHal.createVideoDeviceFile(entry.devFile, entry.w, entry.h, entry.fmt, !isVideoPort));

	// Zero-copy: capture straight into the port's DSP input region (USERPTR).
#ifdef Q_OS_LINUX
	if (mInputMap) {
		const auto base = entry.inputRegion * trikKernel::dspInputBuffersPerRegion;
		QVector<void *> buffers;
		buffers.reserve(static_cast<int>(trikKernel::dspInputBuffersPerRegion));
		for (uint32_t i = 0; i < trikKernel::dspInputBuffersPerRegion; ++i) {
			buffers.append(mInputMap.data() + (base + i) * trikKernel::dspInputFrameSize);
		}
		dev->setUserPtrBuffers(buffers, trikKernel::dspInputFrameSize);
		QLOG_INFO() << "CameraManager: port" << entry.portName << "capturing into region" << entry.inputRegion;
	}
#endif

	if (!dev->open()) {
		QLOG_ERROR() << "CameraManager: failed to open" << entry.devFile;
		return false;
	}

	// Cache the actual negotiated format and bytes-per-line for the DSP.
	entry.negotiatedFourcc = dev->actualFourcc();
	entry.lineLength = dev->bytesPerLine();

	QLOG_DEBUG() << "CameraManager: port" << entry.portName << "opened, actualFourcc=0x" << Qt::hex
		     << dev->actualFourcc() << Qt::dec << "lineLength" << entry.lineLength;

	// Relay frames from the device to the pull slot (both live on the manager
	// thread, so this connection is direct).
	connect(dev.get(), &trikHal::VideoDeviceFileInterface::frameReady, this,
		[this, id](uint32_t bufferIdx, const uint8_t *data, size_t size) {
		onDeviceFrame(id, bufferIdx, data, size);
	});

	entry.dev = std::move(dev);
	entry.dev->fixExposure();
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
	for (uint32_t i = 0; i < inputBuffersPerRegionValue; ++i) {
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

void CameraManager::initSensors()
{
	// Runs in the worker thread (posted from the constructor). Only ov7670
	// ports carry an I2C bus; initVideoSensor() returns false when no sensor is
	// physically wired, so such a port is retried lazily on acquire().
	for (auto &e : mEntries) {
		if (!e.ready || e.i2cBus <= 0 || e.sensorInitialized) {
			continue;
		}
		e.sensorInitialized = mHal.initVideoSensor(e.devFile, e.i2cBus, e.i2cAddress, e.gpioNumber);
	}
}

uint32_t CameraManager::inputBuffersPerRegion() const
{
	return inputBuffersPerRegionValue;
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

uint32_t CameraManager::inputRegion(const QString &port) const
{
	const int id = mPortId.value(port, -1);
	return id >= 0 ? mEntries[id].inputRegion : 0;
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
	result.fourcc = e.fmt;
	result.format = trikKernel::fromV4l2Fourcc(e.negotiatedFourcc);
	result.lineLength = e.lineLength;
	result.inputRegion = e.inputRegion;
	result.inputBuffersPerRegion = inputBuffersPerRegionValue;
	return result;
}

} // namespace trikControl
