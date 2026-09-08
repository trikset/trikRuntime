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

#include <QtCore/QHash>
#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QSharedPointer>
#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>
#include <atomic>
#include <condition_variable>
#include <cstddef>
#include <cstdint>
#include <memory>
#include <mutex>
#include <vector>

#include <trikKernel/videoUtils.h>

#ifdef Q_OS_LINUX
#	include <trikHal/physicalMemoryMapper.h>
#endif

#ifdef TRIK_DEBUG_FPS
#	include <trikKernel/fpsCounter.h>
#endif

namespace trikHal {
class HardwareAbstractionInterface;
class VideoDeviceFileInterface;
}
namespace trikKernel {
class Configurer;
}

namespace trikControl {

class CameraManager;

/// Shared ownership of one captured camera frame.
///
/// A shared (copyable) handle: several consumers can hold the same frame at
/// once. The capture buffer is returned to the driver when the LAST handle
/// referencing it is released (or destroyed), so the buffer is never recycled
/// while somebody is still reading it. The frame's CameraManager is kept alive
/// by the handle for the same duration. A client must not hold a Frame across
/// a release(port)/stopStreaming(port)/close(port) cycle.
///
/// The reference count lives inside the pooled per-port Token (an atomic), so
/// handing frames around allocates nothing on the heap.
class Frame
{
public:
	Frame() = default;
	~Frame()
	{
		release();
	}

	Frame(const Frame &other)
		: mOwner(other.mOwner)
		, mToken(other.mToken)
	{
		if (mToken) {
			mToken->refs.fetch_add(1, std::memory_order_acquire);
		}
	}

	Frame &operator=(const Frame &other)
	{
		if (this != &other) {
			release();
			mOwner = other.mOwner;
			mToken = other.mToken;
			if (mToken) {
				mToken->refs.fetch_add(1, std::memory_order_acquire);
			}
		}
		return *this;
	}

	Frame(Frame &&other) noexcept
		: mOwner(std::move(other.mOwner))
		, mToken(other.mToken)
	{
		other.mToken = nullptr;
	}

	Frame &operator=(Frame &&other) noexcept
	{
		if (this != &other) {
			release();
			mOwner = std::move(other.mOwner);
			mToken = other.mToken;
			other.mToken = nullptr;
		}
		return *this;
	}

	/// Whether the handle actually references a captured frame.
	bool isValid() const
	{
		return mToken != nullptr;
	}

	/// V4L2 buffer index inside the port's capture region. The DSP reads the
	/// frame from in_buff[inputRegion * buffersPerRegion + bufferIndex()].
	uint32_t bufferIndex() const
	{
		return mToken ? mToken->bufferIdx : 0;
	}

	/// Monotonic per-port capture sequence. A consumer uses it to skip frames
	/// it has already processed (latest-wins).
	uint32_t sequence() const
	{
		return mToken ? mToken->seq : 0;
	}

	/// Raw frame bytes (NV16/YUYV), valid while this handle is alive.
	const uint8_t *data() const
	{
		return mToken ? mToken->data : nullptr;
	}

	/// Frame size in bytes.
	size_t size() const
	{
		return mToken ? mToken->size : 0;
	}

	/// Drop this reference to the frame; the buffer is returned to the driver
	/// when the last reference is dropped. Idempotent. Also called by the
	/// destructor.
	void release() noexcept;

private:
	friend class CameraManager;

	/// Per-capture payload, kept in a small pool inside the port's Entry (one
	/// slot per V4L2 buffer). Never heap-allocated per frame.
	/// refs = the manager's "latest" claim (1) + one per outstanding Frame.
	/// The buffer is returned to the driver once refs drops to zero and the
	/// slot is not the current latest anymore.
	struct Token {
		std::atomic<int> refs {0};
		bool captured = false; ///< Manager-thread only: slot holds a dequeued buffer.
		int entryId = -1; ///< mEntries index (for immediate return from Frame::release).
		uint32_t bufferIdx = 0;
		uint32_t seq = 0;
		const uint8_t *data = nullptr;
		size_t size = 0;
	};

	Frame(const QSharedPointer<CameraManager> &owner, Token *token)
		: mOwner(owner)
		, mToken(token)
	{
	}

	QSharedPointer<CameraManager> mOwner; ///< Keeps the manager alive while a frame is held.
	Token *mToken = nullptr; ///< Into CameraManager's per-port pool; valid while mOwner lives.
};

/// Shared camera device manager with a pull-based frame API.
///
/// Clients hold a QSharedPointer<CameraManager> (created by the Brick) and use
/// three operations:
///   - acquire(port) / release(port): refcounted streaming lifecycle. The
///     camera opens and starts streaming on the first acquire, stops on the
///     last release (parked, kept open for a quick re-acquire unless a
///     stopStreaming() latch is left set).
///   - getFrame(port): pull the newest captured frame. Blocking or
///     non-blocking, the client decides when it needs a frame; there are no
///     frame signals.
///   - info() getters: static port configuration + the negotiated format.
///
/// ## Frame delivery (zero-copy pull, no per-frame allocations)
///
/// The manager keeps the newest captured frame of each port. Every getFrame()
/// returns a shared Frame handle referencing that frame; several clients may
/// hold the same frame simultaneously. The capture buffer is returned to the
/// driver (QBUF) when the last handle referencing it is dropped. Frame
/// reference counts live in a small pooled Token array (one slot per V4L2
/// buffer, never allocated per frame); the QBUF itself always happens on the
/// manager thread. A client must not hold a Frame across a
/// release()/stopStreaming()/close() cycle of its port.
///
/// ## Threading
///
/// The manager and its V4L2 devices live on a dedicated worker thread
/// (QSocketNotifier affinity, slow open/I2C I/O never blocks the GUI thread).
/// acquire/release/stopStreaming/close are marshalled there. getFrame() may be
/// called from any thread: the "latest frame" slot is guarded by a small mutex
/// + condition variable, and the frame handles are plain shared pointers.
class CameraManager : public QObject, public QEnableSharedFromThis<CameraManager>
{
	Q_OBJECT

public:
	explicit CameraManager(const trikKernel::Configurer &configurer,
		const trikHal::HardwareAbstractionInterface &hal, QObject *parent = nullptr);
	~CameraManager() override;

	/// Acquire the camera on @p port: open it and start streaming on the first
	/// acquisition, resume a parked camera, bump the refcount otherwise.
	/// Asynchronous; frames flow as soon as the device is streaming, so a
	/// getFrame() started right after returns empty until then. Does NOT clear
	/// a previously latched stopStreaming().
	void acquire(const QString &port);

	/// Acquire by stable port id (no QString lookup).
	void acquire(int portId);

	/// Decrement the refcount of @p port. At zero the stream is stopped; the
	/// device stays open (parked) when a stopStreaming() was latched, otherwise
	/// it is closed and destroyed. A latched stop survives subsequent
	/// acquire()/release() cycles.
	void release(const QString &port);

	/// Release by stable port id (no QString lookup).
	void release(int portId);

	/// Latches a "park on release" request for @p port: when the last client
	/// releases (or right away if the caller is the only client) the stream is
	/// stopped but the device stays open for a quick re-acquire. A later
	/// acquire() resumes streaming but keeps the latch, so the camera parks
	/// again after that client releases.
	void stopStreaming(const QString &port);

	/// Forcibly tear the camera on @p port down (close + destroy the device),
	/// regardless of refcount, and clear the stop latch. Blocks until done.
	void close(const QString &port);

	/// Forcibly tear down all cameras. Blocks until done.
	void close();

	/// Pull the newest captured frame of @p port.
	///
	/// @param afterSeq   only return a frame with sequence() > afterSeq; the
	///                   caller passes its last processed sequence to get
	///                   latest-wins behaviour.
	/// @param timeoutMs  wait up to this long for a newer frame; 0 = return
	///                   immediately (empty Frame if nothing newer), negative =
	///                   wait indefinitely.
	/// @param abort      optional flag; when set, an empty Frame is returned even
	///                   while waiting (the caller re-reads its channel). Set via
	///                   abortFrameWaits() under mFrameLock, so the wait returns
	///                   immediately without a polling slice.
	///
	/// Returns an empty Frame when @p port is unknown/not ready, on timeout,
	/// when aborted, or when the camera is not streaming yet. Blocking waits
	/// sleep on a condition variable, they do not poll.
	Frame getFrame(const QString &port, uint32_t afterSeq = 0, int timeoutMs = 0,
		const std::atomic<bool> *abort = nullptr);

	/// Same as getFrame(QString, ...) but keyed by the stable port id, so the
	/// DSP pipeline's hot path does no QString lookup.
	Frame getFrame(int portId, uint32_t afterSeq = 0, int timeoutMs = 0, const std::atomic<bool> *abort = nullptr);

	/// Abort every pending getFrame() wait and make it return empty, so the
	/// caller re-reads its channel. @p flag is stored true under mFrameLock
	/// before notify_all(): together with the predicate wait in getFrame() this
	/// cannot lose a wakeup, so a channel change takes effect immediately (no
	/// polling slice). Used by the DSP pipeline's setChannel()/stop().
	void abortFrameWaits(std::atomic<bool> *flag);

	/// Static + negotiated per-port info needed to set up the DSP. Thread-safe:
	/// config fields are immutable and the negotiated format is atomic.
	struct PortInfo {
		uint32_t width = 0;
		uint32_t height = 0;
		uint32_t fourcc = 0;
		trikKernel::PixelFormat format = trikKernel::PixelFormat::Unknown;
		uint32_t lineLength = 0;
		uint32_t inputRegion = 0;
		uint32_t inputBuffersPerRegion = 0;
	};
	PortInfo info(const QString &port) const;
	PortInfo info(int portId) const;

	/// Number of capture buffers in one port region.
	uint32_t inputBuffersPerRegion() const;

	/// Number of capture buffers per port region (compile-time constant, shared
	/// with the DSP geometry in trikKernel).
	static constexpr uint32_t inputBuffersPerRegionValue = trikKernel::dspInputBuffersPerRegion;

	/// Capture region index (0..regions-1) assigned to @p port.
	uint32_t inputRegion(const QString &port) const;

	/// Stable 0-based id assigned to @p port, or -1 if the port is unknown.
	int portId(const QString &port) const;

	/// Port name for @p portId (empty when out of range).
	QString portName(int portId) const;

	/// Configured (static) format getters, used e.g. to set up the DSP.
	uint32_t width(const QString &port) const;
	uint32_t height(const QString &port) const;
	uint32_t fourcc(const QString &port) const;

	/// Actual (negotiated) pixel format of @p port, cached at acquire time.
	/// Thread-safe (atomic).
	trikKernel::PixelFormat format(const QString &port) const;

	/// Actual bytes-per-line (V4L2 bytesperline) of @p port. Thread-safe (atomic).
	uint32_t lineLength(const QString &port) const;

	/// Device file path (e.g. "/dev/video0") the @p port is bound to.
	QString deviceFile(const QString &port) const;

	/// mjpg-streamer launcher script path recorded for @p port.
	QString streamerScript(const QString &port) const;

	/// QBUF a capture buffer by entry id. Called from Frame::release() (from
	/// any thread) via queued post; safe because dev->release() is thread-safe
	/// and the entry is never destroyed before the manager.
	void returnBufferToDriverById(int id, uint32_t bufferIdx);

private:
	/// Static per-port state. Config fields are immutable after the constructor
	/// and safe to read from any thread; runtime fields are only touched on the
	/// manager thread (acquire/release/stop/teardown) except `latest`, which is
	/// guarded by mFrameLock.
	struct Entry {
		// -- static config (filled once in the constructor) --
		QString portName;
		QString devFile;
		QString mjpgStreamerScript;
		uint32_t w = 0, h = 0, fmt = 0; ///< Format from the config.
		int i2cBus = 0; ///< ov7670 analog ports only.
		int i2cAddress = 0;
		int gpioNumber = 0;
		bool ready = false; ///< Config for the port is valid.
		uint32_t inputRegion = 0; ///< DSP input region assigned to the port.

		// -- runtime, manager thread only --
		std::unique_ptr<trikHal::VideoDeviceFileInterface> dev;
		int refCount = 0; ///< Number of active clients.
		bool stopLatched = false; ///< stopStreaming() requested; park on last release.
		bool streaming = false; ///< Whether the stream is currently on.
		bool sensorInitialized = false; ///< ov7670 already configured once, so a
		///< re-open (hot-plug) skips init.
		uint32_t captureSeq = 0; ///< Monotonic per-capture counter.

		// -- negotiated at acquire time (manager thread), read cross-thread
		//    only after a frame was observed, so ordering is established by the
		//    release/acquire pair on `latest`. Plain fields keep Entry movable. --
		uint32_t negotiatedFourcc = 0;
		uint32_t lineLength = 0;

		// -- frame delivery, guarded by CameraManager::mFrameLock --
		/// Pool of one Token per capture buffer, allocated once in makeEntry.
		/// A token is only ever reused for the same V4L2 buffer index, and only
		/// when its refs are zero (a claimed buffer is out of the driver, so it
		/// cannot be recaptured while a Frame still references it).
		std::unique_ptr<Frame::Token[]> tokens;
		Frame::Token *latest = nullptr; ///< The newest captured frame (manager's claim).
	};

	/// Build a port Entry from the config (no device is opened). Private static
	/// so the config parsing stays out of the constructor's hot body.
	static Entry makeEntry(const trikKernel::Configurer &configurer, const QString &port, uint32_t region);

	/// Opens (or reuses) the device of @p id using its static config format and
	/// bumps its refcount. Runs in the manager's thread.
	bool openDeviceLocked(int id, Entry &entry);

	/// Relays a frame captured by the device of @p id to the pull slot. Runs in
	/// the manager's thread (direct connection from the device).
	void onDeviceFrame(int id, uint32_t bufferIdx, const uint8_t *data, size_t size);

	/// QBUF a capture buffer back to the driver. Runs in the manager's thread.
	void returnBufferToDriver(Entry &entry, uint32_t bufferIdx);

	/// Drop the manager's "latest" claim of @p entry and clear the slot. The
	/// buffer itself is NOT returned: callers do this right after STREAMOFF,
	/// which already returned every buffer to the driver. Runs on the manager
	/// thread (with mFrameLock).
	void dropLatest(Entry &entry);

#ifdef Q_OS_LINUX
	/// Map the capture region (fixed physical address) into host virtual memory
	/// via /dev/mem, so the VPIF DMA engine can capture straight into it.
	/// Runs in the constructor. Returns true on success.
	bool mapInputRegion();
#endif

	/// Stop, close and destroy the open device of a single port, reset its
	/// refcount/streaming/latch. Runs in the manager's thread.
	void tearDownPort(int id);

	/// Eagerly initialize every ov7670 analog port off the GUI thread.
	void initSensors();

	/// Runs @p fn in the manager's thread, blocking the caller if it lives in
	/// another thread. Used by the destructor and close(), which must not return
	/// before the devices are torn down in their own thread.
	template<typename Fn>
	void runInManagerThread(Fn &&fn)
	{
		if (QThread::currentThread() == thread()) {
			fn();
			return;
		}
		QMetaObject::invokeMethod(this, std::forward<Fn>(fn), Qt::BlockingQueuedConnection);
	}

	/// Runs @p fn in the manager's thread (non-blocking; runs directly if the
	/// caller is already there).
	template<typename Fn>
	void runAsync(Fn &&fn)
	{
		if (QThread::currentThread() == thread()) {
			fn();
			return;
		}
		QMetaObject::invokeMethod(this, std::forward<Fn>(fn), Qt::QueuedConnection);
	}

	const trikHal::HardwareAbstractionInterface &mHal;

	/// Worker thread the manager and its devices live on.
	QScopedPointer<QThread> mThread;

	/// Port name -> stable port id. Immutable after construction; read from any
	/// thread. Entries live in mEntries.
	QHash<QString, int> mPortId;

	/// Per-port state, indexed by port id. Never resized after construction, so
	/// entries stay stable for the manager's whole lifetime.
	std::vector<Entry> mEntries;

	/// Guards every Entry::latest (the "newest frame" slot) and provides the
	/// wait for getFrame(). Taken only around the pointer swap / the wait.
	mutable std::mutex mFrameLock;
	std::condition_variable mFrameCond;

#ifdef Q_OS_LINUX
	/// RAII-mapped capture region (mmap'd /dev/mem). Written once in the
	/// constructor; munmaps itself on destruction.
	trikHal::MappedMemory mInputMap;
#endif

#ifdef TRIK_DEBUG_FPS
	/// TEMP DEBUG: total DQBUF rate across ports (logged every few seconds).
	trikKernel::FpsCounter mCaptureFps {QStringLiteral("CameraManager capture")};
#endif
};

} // namespace trikControl
