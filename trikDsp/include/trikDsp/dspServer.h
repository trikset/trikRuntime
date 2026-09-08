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

#include <QtCore/QObject>
#include <QtCore/QProcess>
#include <QtCore/QScopedPointer>

#include "dspSource.h"
#include "dspTypes.h"
#include "trikDspDeclSpec.h"

namespace trikHal {
class FbOutputInterface;
}

namespace trikDsp {

/// ARM <-> DSP bridge via TI IPC MessageQ over RPMsg.
///
/// ## Lifecycle & thread model
///
/// Construction is cheap - only the Impl is heap-allocated, no IPC is started.
/// Caller MUST call init() to bring up the IPC stack (LAD daemon -> Ipc_start ->
/// MessageQ -> shared buffers).  init() is a **synchronous blocking** call that
/// spins a local QEventLoop; the calling thread's event loop keeps running
/// during the wait.  When init() returns the object has either emitted
/// successfullyInited() (ready for use) or errorOccurred() (permanent failure).
///
/// After init() the object is used exclusively from a single worker thread
/// (the caller's frame-processing loop): every public method below is a plain
/// synchronous call, there are no signals in the frame path and no queued
/// handoffs.
///
/// Destructor terminates the LAD daemon (terminate() -> kill() fallback).
/// Impl::~Impl() tears down IPC (destroyMessageQueue -> Ipc_stop -> munmap).
///
/// ## Frame processing
///
/// processFrame() runs one blocking STEP on the DSP for @p bufferIdx (the flat
/// DSP input buffer index) using @p channel's algorithm and parameters. On
/// success it fills @p out: for Jpeg, out.jpegData points at the encoded bytes
/// in the shared output buffer (valid only until the next processFrame() call),
/// so the JPEG consumer must consume them synchronously on the DSP thread right
/// after the call. When channel.videoOut is set the 240x240 video frame is
/// written to the attached framebuffer output inside processFrame().
///
/// The algorithm is (re)registered automatically whenever the algorithm, pixel
/// format or line length of the channel changes.
class TRIKDSP_EXPORT DspServer : public QObject
{
	Q_OBJECT

public:
	/// Constructs the object WITHOUT IPC initialisation.
	/// @param rprocId  remoteproc ID of the DSP core (0 for OMAP-L138).
	/// @param parent   optional QObject parent.
	///
	/// @post  mLadProcess default-constructed (unstarted).
	/// @post  d->rprocId set, nothing else allocated.
	explicit DspServer(uint16_t rprocId, QObject *parent = nullptr);

	/// Terminates the LAD daemon and destroys IPC resources.
	~DspServer() override;

	/// Synchronously bring up the IPC stack.  BLOCKING.
	///
	/// Sequence: start lad_omapl138 (QProcess) -> wait up to 15s for start ->
	/// Ipc_start() -> setupMessageQueue() -> mapSharedBuffers().
	///
	/// Emits errorOccurred(msg) on failure, successfullyInited() on success.
	///
	/// @warning Caller MUST connect to errorOccurred and successfullyInited
	///          BEFORE calling init().  These signals fire synchronously during
	///          this call.
	void init();

	/// Process the frame captured into V4L2 buffer @p bufferIdx with the given
	/// @p channel. Blocking: waits for the DSP's STEP response. The DSP reads
	/// the frame from in_buff[channel.inputBufferBase + bufferIdx].
	///
	/// Re-registers the algorithm on the DSP when @p channel's algorithm, pixel
	/// format or line length differ from the previous call. On success fills
	/// @p out and, for Jpeg, points out.jpegData at the encoded bytes in the
	/// shared output buffer. When @p channel.videoOut is set, writes the
	/// framebuffer output.
	///
	/// MUST be called from a single worker thread (the frame-processing loop).
	///
	/// @return true if the frame was processed successfully.
	bool processFrame(const DspChannel &channel, OutArgs &out, uint32_t bufferIdx);

	/// Enable/disable the video framebuffer output. Opens the fb on enable
	/// (idempotent), closes it on disable. Must be called before the first
	/// processFrame() with a videoOut channel; may be called any time from the
	/// worker thread.
	void setVideoOutput(bool enabled);

	/// Attach a HAL framebuffer output for direct video display.
	/// Must be called before init(). DspServer takes ownership.
	void setFbOutput(trikHal::FbOutputInterface *fb);

Q_SIGNALS:
	/// @name Lifecycle signals
	/// @{

	/// Emitted synchronously from init() when IPC initialisation fails.
	void errorOccurred(const QString &message);

	/// Emitted synchronously from init() when IPC initialisation succeeds.
	void successfullyInited();

	/// @}

private:
	class Impl;
	QScopedPointer<Impl> d;

	QProcess mLadProcess;
};

}
