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

#include <QtCore/QAbstractItemModel>
#include <QtCore/QHash>
#include <QtCore/QMutex>
#include <QtCore/QObject>
#include <QtCore/QVariantList>

#include <trikControl/trikControlDeclSpec.h>
#include <trikControl/videoPreviewSink.h>
#include <trikControl/videoPreviewSource.h>

namespace trikControl {

class VideoPreviewModel;

/// Concrete latest-wins store shared by preview producers and consumers.
///
/// The hub implements both sides of the pipeline over one thread-safe
/// dictionary: video sensors write through VideoPreviewSink, while the display
/// reads through VideoPreviewSource and its signals. Each active id keeps only
/// its newest frame, so a slow consumer never builds a backlog.
///
/// The hub is normally created once by the composition root on the GUI thread
/// and held by a shared_ptr for the whole application lifetime. The same
/// instance is registered as the "VideoPreviewHub" QML singleton. Producers and
/// consumers themselves depend only on the sink/source interfaces.
///
/// @section threading Threading
///
/// All public methods are thread-safe. producer methods may be called from
/// multiple sensor worker threads; consumer methods may be called from the GUI
/// or render thread. Signals are emitted after the internal lock has been
/// released, so slots may safely call activeIds()/latest(). A signal emitted by
/// a worker thread is delivered to GUI objects through a queued connection.
///
/// @sa VideoPreviewSink, VideoPreviewSource
class TRIKCONTROL_EXPORT VideoPreviewHub : public VideoPreviewSource, public VideoPreviewSink
{
	Q_OBJECT
	Q_PROPERTY(QVariantList activeIds READ activeIds NOTIFY activeChanged)
	Q_PROPERTY(QAbstractItemModel *previewModel READ previewModel CONSTANT)

public:
	/// Creates an empty store. @p parent is passed to QObject.
	///
	/// For QML registration and queued signal delivery the object should live on
	/// the GUI thread. A single instance per application is expected.
	explicit VideoPreviewHub(QObject *parent = nullptr);

	/// @copydoc VideoPreviewSink::openPreview
	///
	/// Allocates a new id on every call and emits activeChanged().
	int openPreview() override;

	/// @copydoc VideoPreviewSink::closePreview
	///
	/// Emits activeChanged() only when @p id was active.
	void closePreview(int id) override;

	/// @copydoc VideoPreviewSink::publish
	///
	/// The new frame becomes visible to latest() before frameReady(@p id) is
	/// emitted.
	void publish(int id, const VideoPreviewFrame &frame) override;

	/// @copydoc VideoPreviewSource::latest
	///
	/// The returned value is a copy; its FrameBufferPtr keeps the dma-buf alive
	/// for as long as the caller holds the frame.
	VideoPreviewFrame latest(int id, quint64 *sequence) const override;

	/// @copydoc VideoPreviewSource::activeIds
	///
	/// Ids are sorted in ascending order. This is also the Q_PROPERTY getter
	/// used by QML; activeChanged() is its NOTIFY signal.
	QVariantList activeIds() const override;

	/// Incremental model of the currently open preview ids.
	///
	/// Unlike activeIds(), which is a snapshot rebuild on every change, this
	/// model emits row insertions/removals, so a Repeater keeps untouched
	/// delegates alive when one sensor is opened or stopped.
	QAbstractItemModel *previewModel() const;

Q_SIGNALS:
	/// Emitted after a new preview id has been added to previewModel().
	void previewOpened(int id);

	/// Emitted after @p id has been removed from previewModel().
	void previewClosed(int id);

private:
	/// Per-id latest frame and its publication counter.
	struct Entry {
		VideoPreviewFrame frame;
		/// Monotonic per-id publish counter. Consumers compare it to detect a
		/// new frame without inspecting the dma-buf contents.
		quint64 publishSequence = 0;
	};

	/// Rebuilds mActiveIds from mEntries. The caller must hold mMutex.
	void rebuildActiveIdsLocked();

	/// Guards mEntries, mActiveIds and mNextId. Never held while emitting a
	/// signal.
	mutable QMutex mMutex;

	/// Next id to hand out from openPreview(). Monotonic, so an id is never
	/// reused while the hub is alive.
	int mNextId = 0;

	/// Active previews keyed by the opaque id returned from openPreview().
	QHash<int, Entry> mEntries;

	/// Sorted snapshot exposed to consumers and QML. Rebuilt under mMutex.
	QVariantList mActiveIds;

	/// Incremental preview-id model fed by previewOpened()/previewClosed().
	VideoPreviewModel *mPreviewModel = nullptr;
};

} // namespace trikControl
