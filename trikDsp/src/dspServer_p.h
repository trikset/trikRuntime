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

#include <QtCore/QScopedPointer>

#ifdef Q_OS_LINUX
#	include <trikHal/physicalMemoryMapper.h>
#endif
#include <trik/buffer.h>
#include <trik/sensors/cv_algorithm.h>
#include <trik/sensors/cv_algorithm_args.h>

#include "dspServer.h"

struct MessageQ_Object;
struct trik_msg;

namespace trikHal {
class FbOutputInterface;
}

// The ARM side (trikKernel geometry) and the DSP firmware (trik/buffer.h) must
// agree on the capture layout: the ARM maps `in_buff` and feeds its addresses
// to V4L2 USERPTR, the DSP reads the same region by flat index. Catch any drift
// here, at compile time, in the one place that sees both headers.
static_assert(trikKernel::dspInputWidth == IMG_WIDTH, "dspInputWidth drifted from the DSP firmware (IMG_WIDTH)");
static_assert(trikKernel::dspInputHeight == IMG_HEIGHT, "dspInputHeight drifted from the DSP firmware (IMG_HEIGHT)");
static_assert(trikKernel::dspInputFrameSize == BUFFER_SIZE,
	"dspInputFrameSize drifted from the DSP firmware (BUFFER_SIZE)");
static_assert(trikKernel::dspInputRegions == TRIK_INPUT_REGIONS,
	"dspInputRegions drifted from the DSP firmware (TRIK_INPUT_REGIONS)");
static_assert(trikKernel::dspInputBuffersPerRegion == TRIK_INPUT_BUFFERS,
	"dspInputBuffersPerRegion drifted from the DSP firmware (TRIK_INPUT_BUFFERS)");
static_assert(trikKernel::dspInputBufferTotal == TRIK_INPUT_TOTAL,
	"dspInputBufferTotal drifted from the DSP firmware (TRIK_INPUT_TOTAL)");

namespace trikDsp {

/// TI IPC implementation (pimpl for DspServer).
///
/// ## Responsibility
///
/// Impl encapsulates all interaction with the TI IPC stack:
///   - MessageQ (host <-> slave queue pair for request/response)
///   - /dev/mem mmapping for DSP shared buffers
///   - Algorithm registration and frame processing
///
/// DspServer owns one Impl via QScopedPointer.
///
/// ## Initialisation sequence (called from DspServer::init)
///
///   startIpc()         -> Ipc_transportConfig + Ipc_start
///   setupMessageQueue() -> MessageQ_create (host) + retry-loop MessageQ_open (slave)
///   mapSharedBuffers()  -> INIT command over MessageQ, mapPhysicalMemory (output)
///
/// If any step fails the object is left partially initialised.
/// ~Impl() safely tears down whatever was allocated:
///   destroyMessageQueue() -> null-safe (checks mHostQue / mSlaveQue)
///   Ipc_stop()            -> safe even without Ipc_start
///   mOutMap (RAII)        -> munmaps the output region by itself
///
/// ## Concurrency contract
///
/// Impl has NO thread safety mechanisms of its own. DspServer::processFrame()
/// is a blocking call that MUST be invoked from a single worker thread (the
/// caller's frame-processing loop). No queued handoffs happen inside Impl.
///
/// ## MessageQ protocol
///
/// Every command follows the same pattern:
///   allocRequest(hostQue, heapId, size, cmd) -> sets cmd + reply queue
///   MessageQ_put(slaveQue, msg)
///   MessageQ_get(hostQue, ..., MessageQ_FOREVER) -> blocks until response
///   freeMessage(response)
///
/// MessageQ_FOREVER blocks the calling thread - there is no timeout.
/// This is acceptable because the DSP responds within microseconds
/// and a non-response indicates a DSP crash (fatal).
///
/// ## Resource ownership
///
///   mHostQue:   created by MessageQ_create  -> deleted in destroyMessageQueue
///   mSlaveQue:   opened by MessageQ_open     -> closed in destroyMessageQueue
///   mOutMap:    RAII owner of the DSP output mapping -> munmaps in ~MappedMemory
///   mDspOut:     points into the mOutMap region (not a separate allocation)
class DspServer::Impl
{
public:
	/// Tears down IPC in reverse order:
	///   destroyMessageQueue -> Ipc_stop (mOutMap unmaps itself via RAII)
	///
	/// All steps are null-safe - the object may be in any state
	/// (fully initialised, partially initialised, never initialised).
	~Impl();

	/// Configure the RPMsg transport and call Ipc_start().
	/// @return true on success.  On failure the caller should destroy.
	bool startIpc();

	/// Create the host MessageQ and open the slave queue.
	/// The slave queue may not be available immediately (DSP startup),
	/// so MessageQ_open is retried with 1-second sleeps (10 retries).
	/// @return true if both queues are ready, false on timeout/error.
	/// @note  Returns bool explicitly (was UB before fix).
	bool setupMessageQueue();

	/// Close the slave queue and delete the host queue.
	/// Safe to call regardless of initialisation state.
	void destroyMessageQueue();

	/// Send the INIT command to DSP, receive the physical address of the
	/// shared output buffer, and mmap it via /dev/mem (RAII-owned by mOutMap).
	///
	/// @return true if the output buffer was successfully mapped.
	bool mapSharedBuffers();

	/// Register a CV algorithm on the DSP.
	/// Sends the algorithm descriptor (pixel format, line length) via MessageQ.
	/// Must be called before the first step() for a new algorithm type.
	///
	/// @param algo  public API algorithm enum (converted to DSP command).
	/// @param desc  pixel format and bytes-per-line of the video source.
	void registerAlgorithm(Algorithm algo, const AlgoDescriptor &desc);

	/// Execute one frame-processing STEP on the DSP.
	/// Sends the input buffer index plus the input args to the DSP, blocks
	/// until response, copies output args.
	///
	/// @param in        sensor parameters (HSV ranges, matrix sizes, etc.).
	/// @param out       DSP processing results (locations, colours).
	/// @param bufferIdx index of the DSP input buffer holding the frame.
	/// @return true on success.
	bool step(const InArgs &in, OutArgs &out, uint32_t bufferIdx);

	/// @name Algorithm cache (avoids re-registration in the hot path)
	/// @{
#ifndef TRIK_DSP_STUB
	enum trik_cv_algorithm currentAlgo() const
	{
		return mCurrentAlgo;
	}
	PixelFormat currentFormat() const
	{
		return mCurrentFormat;
	}
	uint32_t currentLineLength() const
	{
		return mCurrentLineLength;
	}
	void setCurrentAlgo(enum trik_cv_algorithm algo, PixelFormat format, uint32_t lineLength)
	{
		mCurrentAlgo = algo;
		mCurrentFormat = format;
		mCurrentLineLength = lineLength;
	}
#endif
	/// @}

#ifndef TRIK_DSP_STUB
	/// Start of the DSP shared output buffer holding the last processed frame
	/// (a JPEG stream for the JPEG encoder, the 240x240 video frame otherwise).
	const uint8_t *outStart() const
	{
		return static_cast<const uint8_t *>(mDspOut.start);
	}
#endif

	/// HAL framebuffer output (optional, set via setFbOutput). Owned here: it is
	/// created by the HAL (createFbOutput) with no parent and destroyed in ~Impl.
	QScopedPointer<trikHal::FbOutputInterface> mFbOutput;

	/// 	TI remoteproc ID of the DSP core (0 for OMAP-L138).
	uint16_t rprocId = 0;

private:
	/// Send a MessageQ request and block until the DSP responds.
	/// @return the response message, or nullptr on failure.
	/// @note  Blocks with MessageQ_FOREVER - no timeout.
	::trik_msg *sendAndWaitForResponse(::trik_msg *msg);

	/// Free a MessageQ-allocated message.
	static void freeMessage(::trik_msg *msg);

#ifndef TRIK_DSP_STUB
	/// Last registered algorithm.  Used to avoid re-registration in processFrame.
	enum trik_cv_algorithm mCurrentAlgo = TRIK_CV_ALGORITHM_NONE;
	/// Pixel format the DSP algorithm was registered with (part of the cache).
	PixelFormat mCurrentFormat = PixelFormat::Unknown;
	/// Bytes-per-line the DSP algorithm was registered with (part of the cache).
	uint32_t mCurrentLineLength = 0;

	/// TI IPC host queue handle (created by MessageQ_create).
	struct MessageQ_Object *mHostQue = nullptr;

	/// TI IPC slave queue handle (opened by MessageQ_open).
	/// Initialised to MessageQ_INVALIDMESSAGEQ (0xffff).
	unsigned mSlaveQue = 0xffff;

	/// DSP shared output buffer (mmap'd /dev/mem); start points into mOutMap.
	struct buffer mDspOut = {};

	/// RAII owner of the DSP output mapping; munmaps itself on destruction.
	trikHal::MappedMemory mOutMap;
#endif
};

}
