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

#include <QtCore/QVector>
#include <QtQuick/QQuickItem>

#include <trikHal/frameBuffer.h>

class QSGImageNode;
class QSGNode;
class QTimer;

namespace trikControl {
class VideoPreviewSource;
}

namespace trikGui {

class VideoOverlayTexture;

/// Live preview of all active video sensors, composited by the Qt Quick scene
/// graph.
///
/// Every active sensor gets a video quad on screen. When the client also
/// publishes annotations (the ColorSensor grid, for example), a transparent
/// overlay quad is created lazily above it. Clients that only update a frame in
/// place never pay for overlay resources. The frame is not copied through the
/// CPU: the producer writes into a dma-buf and the item imports that buffer as
/// a GL texture through EGLImage (GlDmaTexture). The overlay is rasterized on
/// the CPU and uploaded as a normal RGBA texture (VideoOverlayTexture).
///
/// @section sceneGraph Scene-graph lifetime
///
/// A QQuickItem does not draw by itself. It returns a QSGNode tree from
/// updatePaintNode() and Qt renders that tree. All GL/EGL resources therefore
/// live inside the tree:
///
///   * each Entry has an optional overlay QSGImageNode that owns its
///     VideoOverlayTexture; it exists only for clients that publish overlays;
///   * each Entry has a TextureCache node that owns the imported GlDmaTexture
///     objects;
///   * the video QSGImageNode only references a texture owned by TextureCache.
///
/// When the set of active sensors changes, the old root and its whole subtree
/// are deleted and a new root is built. Deleting a QSGNode deletes its children
/// (QSGNode::OwnedByParent), so this happens on the render thread with the GL
/// context current, which is exactly where GL resources must be released. The
/// item destructor never touches these resources; Qt deletes the returned root
/// later on the render thread.
///
/// @section threading Threading
///
/// updatePaintNode() runs on the scene-graph render thread. All scene-graph
/// access, texture creation and overlay uploads happen there. The ticker and
/// the source connections only wake the render thread up; VideoPreviewSource is
/// documented as safe to call from any thread.
///
/// @section frameLifetime Frame lifetime
///
/// A frame may still be read by the GPU after it has been replaced on screen.
/// The previously displayed FrameBufferPtr is therefore retired and released
/// only after QQuickWindow::frameSwapped(), when the presented frame is done.
class VideoPreviewOverlayItem : public QQuickItem
{
	Q_OBJECT

	/// The VideoPreviewSource that provides active ids and the latest frames.
	/// Exposed as a QObject so QML can assign a hub without knowing its type.
	Q_PROPERTY(QObject *source READ source WRITE setSource NOTIFY sourceChanged)

	/// Height of all previews including the gaps between them, in pixels.
	/// QML binds the overlay container's height to this value. It follows the
	/// real frame aspect ratio, so it changes only when the frame size changes.
	Q_PROPERTY(int contentHeight READ contentHeight NOTIFY contentHeightChanged)

public:
	explicit VideoPreviewOverlayItem(QQuickItem *parent = nullptr);
	~VideoPreviewOverlayItem() override = default;

	QObject *source() const;
	void setSource(QObject *source);

	int contentHeight() const;

Q_SIGNALS:
	void sourceChanged();
	void contentHeightChanged();

protected:
	/// Called on the render thread to synchronise the scene-graph subtree with
	/// the current sensor set and frames.
	QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;

private:
	/// Scene-graph node that owns the dma-buf textures of one entry.
	///
	/// It is a child of the item's root node, so it is destroyed (with a current
	/// GL/EGL context) together with the root. Textures are cached by dma-buf
	/// fd because a producer cycles through a small, fixed set of buffers and
	/// importing the same buffer on every frame would be expensive.
	class TextureCache;

	/// Per-sensor state. Exactly one entry exists for every id in mIds.
	struct Entry {
		/// Stable preview id, normally CameraManager::portId().
		int id = -1;

		/// Sequence of the last frame that was applied to the scene graph.
		/// Used to avoid marking the video material dirty on every tick.
		quint64 sequence = 0;

		/// Generation of the overlay currently uploaded to overlayTexture.
		/// Zero means the client never published an overlay for this entry.
		quint32 overlayGeneration = 0;

		/// False until the first overlay of this entry has been uploaded.
		bool overlayInitialized = false;

		/// Format of the frames currently shown. A change invalidates the
		/// per-entry texture cache because retyped/reused buffers would
		/// otherwise be sampled with stale import parameters.
		trikHal::FrameFormat format;

		/// Frame currently displayed. Kept alive until frameSwapped(), because
		/// the GPU may still be sampling it.
		trikHal::FrameBufferPtr currentBuffer;

		/// Owns the imported video textures. A scene-graph child of the root.
		TextureCache *textures = nullptr;

		/// Created together with overlayNode; owned by it
		/// (setOwnsTexture(true)), not by the item. Null for video-only clients.
		VideoOverlayTexture *overlayTexture = nullptr;

		/// Created lazily on the first valid frame, because an image node must
		/// never be added to the scene graph without a texture.
		QSGImageNode *videoNode = nullptr;

		/// Created lazily on the first frame that carries an overlay, and drawn
		/// above the video. Null for video-only clients.
		QSGImageNode *overlayNode = nullptr;
	};

	/// Wakes the render thread at a fixed rate, collapsing bursts of
	/// frameReady() signals into one scene-graph update.
	void tick();

	/// Builds a fresh Entry subtree for @p ids under @p root.
	///
	/// Must only be called after any previous root has been destroyed, so no
	/// Entry may still reference nodes from the old tree.
	void rebuildEntries(QSGNode *root, const QVector<int> &ids);

	/// Refreshes one sensor from its latest frame: picks (or imports) the video
	/// texture, re-rasterizes the overlay when it changed, and positions both
	/// quads. Does nothing while the sensor has no valid frame yet.
	///
	/// @return the slot height used for this sensor, derived from the frame
	/// aspect ratio (or the bootstrap size before the first frame).
	int updateEntry(Entry &entry, QSGNode *root, qreal y);

	/// Posts a contentHeight() change to the GUI thread. Called from the render
	/// thread; the queued call is skipped when @p height did not change.
	void updateContentHeight(int height);

	/// Makes @p buffer the entry's current frame. The previously displayed
	/// buffer is retired and released after the next frameSwapped(), when the
	/// GPU is done with the frame that used it.
	void advanceFrame(Entry &entry, const trikHal::FrameBufferPtr &buffer);

	/// Creates the video node on first use. Inserted below the overlay when the
	/// client has one, otherwise appended directly to @p root. Called only once
	/// a valid texture exists for the sensor.
	void ensureVideoNode(Entry &entry, QSGNode *root);

	/// Releases the buffers referenced by the old node tree before it is
	/// destroyed.
	///
	/// When @p keepUntilSwap is true the old tree was still visible, so its
	/// buffers are kept until frameSwapped(). When it is false the tree is
	/// already gone (first update or context loss), so all retired buffers are
	/// dropped immediately.
	void retireCurrentBuffers(bool keepUntilSwap);

	/// Ids of the sensors currently shown, in ascending order. Compared with
	/// the source's activeIds() on every update to detect tree rebuilds.
	QVector<int> mIds;

	/// One entry per id in mIds, in the same order as mIds.
	QVector<Entry> mEntries;

	/// Buffers retired from the visible frame. Released on frameSwapped(), i.e.
	/// after the render thread has presented the frame that used them.
	QVector<trikHal::FrameBufferPtr> mPendingRelease;

	/// Fixed-rate update ticker owned by the item.
	QTimer *mTicker = nullptr;

	/// The QML-assigned source object and its casted form. mSourceObject keeps
	/// the exact pointer that was assigned; mSource is null when the object does
	/// not implement VideoPreviewSource.
	QObject *mSourceObject = nullptr;
	trikControl::VideoPreviewSource *mSource = nullptr;

	/// Set when the source reports a new frame or a changed sensor set. The
	/// ticker consumes it and calls update() at most once per tick.
	bool mUpdatePending = true;

	/// True once the item is connected to its window's frameSwapped() signal.
	bool mSwappedConnected = false;

	/// Current value of the contentHeight property. Read and written on the
	/// GUI thread only.
	int mContentHeight = 0;

	/// Last height posted from the render thread, used to avoid redundant
	/// queued GUI updates. Render thread only.
	int mPostedContentHeight = -1;
};

} // namespace trikGui
