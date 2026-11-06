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

#include <trikControl/trikControlDeclSpec.h>
#include <trikControl/videoPreviewTypes.h>

namespace trikControl {

/// Write side of the video preview pipeline.
///
/// A producer (for example ColorSensorWorker or a future algorithm) depends
/// only on this interface: it registers the preview, pushes the newest frame
/// and unregisters when done. Reading frames and change notifications are
/// described by VideoPreviewSource, so producers do not need Qt signals, QML or
/// the concrete store.
///
/// Semantically the sink is non-owning: the composition root creates it and
/// keeps it alive for the whole application lifetime. The sink is passed to
/// producers as a shared_ptr so a worker thread may safely keep a reference
/// while it is publishing; producers use it as a service and never delete it.
///
/// @section threading Threading
///
/// All methods may be called from sensor worker threads. Implementations must
/// serialize concurrent calls and must not block publish() on a display
/// consumer: the frame is latest-wins, so a slow reader only loses intermediate
/// frames.
///
/// @sa VideoPreviewSource, VideoPreviewHub
class TRIKCONTROL_EXPORT VideoPreviewSink
{
public:
	virtual ~VideoPreviewSink() = default;

	/// Registers a new preview and returns its unique id.
	///
	/// The id is an opaque handle owned by the returned value: it is not
	/// related to a camera port or a sensor type. Every call allocates a new id,
	/// so two producers may publish concurrently even when they share a camera.
	/// After this call publish() stores frames for the returned id and consumers
	/// can request them through VideoPreviewSource.
	///
	/// Thread-safe.
	virtual int openPreview() = 0;

	/// Unregisters @p id and drops its latest frame. Idempotent.
	///
	/// After this call publish() for @p id is a no-op and the stored
	/// FrameBufferPtr is released, so the producer's buffer pool may recycle it.
	///
	/// Thread-safe.
	virtual void closePreview(int id) = 0;

	/// Stores @p frame as the newest frame for @p id.
	///
	/// The value is copied (trikHal::FrameBufferPtr and the overlay are shared,
	/// not deep-copied), overwriting any previous frame for @p id. No queue is
	/// built: a consumer that falls behind observes the newest frame only.
	///
	/// The frame is ignored when @p id is not active. The producer may release
	/// its own reference to the frame immediately after the call; the stored
	/// FrameBufferPtr keeps the dma-buf alive until the frame is replaced or the
	/// preview is deactivated.
	///
	/// Thread-safe.
	virtual void publish(int id, const VideoPreviewFrame &frame) = 0;
};

} // namespace trikControl
