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
#include <QtCore/QVariantList>

#include <trikControl/trikControlDeclSpec.h>
#include <trikControl/videoPreviewTypes.h>

namespace trikControl {

/// Read-only side of the video preview pipeline.
///
/// A display consumer (for example VideoPreviewStreamItem) depends only on
/// this interface: it asks for the latest frame or the set of active ids and
/// subscribes to change notifications. Frame production is described by
/// VideoPreviewSink, so producers do not need Qt signals, QML or the concrete
/// store.
///
/// The implementation is a QObject owned by the composition root (normally a
/// shared_ptr held by the GUI entry point); this interface is non-owning.
/// Consumers keep a raw pointer, never delete it, and must not outlive the
/// owner.
///
/// @section threading Threading
///
/// Implementations must allow latest() and activeIds() to be called from any
/// thread. Signals are emitted by the producer thread after the internal state
/// has been updated, so consumers living in another thread receive them through
/// a queued connection. Consumers must not rely on the source's QObject thread
/// affinity.
///
/// @sa VideoPreviewSink, VideoPreviewHub
class TRIKCONTROL_EXPORT VideoPreviewSource : public QObject
{
	Q_OBJECT

public:
	/// Creates the source. @p parent is passed to QObject.
	///
	/// The class is abstract: only concrete stores such as VideoPreviewHub are
	/// instantiated.
	explicit VideoPreviewSource(QObject *parent = nullptr)
		: QObject(parent)
	{
	}

	/// Returns the newest frame published for @p id.
	///
	/// When @p sequence is not null it receives the frame's production
	/// sequence (0 when @p id is not active). The returned frame owns a
	/// reference to its dma-buf through trikHal::FrameBufferPtr, so the caller
	/// may hold it for as long as needed without blocking the producer.
	///
	/// Lookups never wait for a frame: if nothing has been published for
	/// @p id yet, an invalid frame (null buffer, empty overlay) is returned.
	///
	/// Thread-safe.
	virtual VideoPreviewFrame latest(int id, quint64 *sequence) const = 0;

	/// Ids of all currently active previews, in ascending order.
	///
	/// The returned list is a snapshot: it stays valid until activeChanged()
	/// is emitted. QVariantList is used so the same value can be exposed to QML
	/// without conversion.
	///
	/// Thread-safe.
	virtual QVariantList activeIds() const = 0;

Q_SIGNALS:
	/// Emitted after a new frame has been stored for @p id.
	///
	/// The payload is intentionally only the id: consumers pull the actual
	/// frame with latest(). The signal is emitted from the producer thread
	/// (normally a video sensor worker), outside any internal lock, so a slot in
	/// the GUI thread runs as a queued call.
	void frameReady(int id);

	/// Emitted when a preview is activated or deactivated.
	///
	/// Emitted after activeIds() already reflects the new set, outside any
	/// internal lock; it is safe to call activeIds()/latest() from the slot.
	/// This is also the NOTIFY signal of VideoPreviewHub::activeIds.
	void activeChanged();
};

} // namespace trikControl
