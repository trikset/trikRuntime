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

#include <QtCore/QPointer>
#include <QtCore/QVector>
#include <QtQuick/QQuickItem>

#include <trikHal/frameBuffer.h>

class QSGImageNode;
class QSGNode;
class QQuickWindow;

namespace trikControl {
class VideoPreviewSource;
}

namespace trikGui {

class VideoOverlayTexture;

/// Live preview of one video stream, composited by the Qt Quick scene graph.
///
/// The item renders exactly one preview id: a zero-copy dma-buf video quad plus
/// an optional annotation overlay quad above it. QML creates one instance per
/// active id (normally through a Repeater), so standard QQuickItem properties
/// such as width, height, rotation and transform apply to the whole stream.
///
/// The video frame is not copied through the CPU: the producer writes into a
/// dma-buf and the item imports that buffer as a GL texture through EGLImage
/// (GlDmaTexture). The overlay is rasterized on the CPU and uploaded as a normal
/// RGBA texture (VideoOverlayTexture).
///
/// @section frameLifetime Frame lifetime
///
/// A frame may still be read by the GPU after it has been replaced on screen.
/// The previously displayed FrameBufferPtr is therefore retired and released
/// only after QQuickWindow::frameSwapped(). When the item is destroyed while it
/// still owns frames, they are handed to a short-lived helper that survives one
/// frame swap, so a stopped stream cannot recycle a buffer that is still sampled.
class VideoPreviewStreamItem : public QQuickItem
{
	Q_OBJECT

	/// The VideoPreviewSource that provides the latest frame for this stream.
	Q_PROPERTY(QObject *source READ source WRITE setSource NOTIFY sourceChanged)

	/// Opaque id returned by VideoPreviewSink::openPreview().
	Q_PROPERTY(int previewId READ previewId WRITE setPreviewId NOTIFY previewIdChanged)

	/// Width/height ratio of the latest frame (or the bootstrap 4:3 before the
	/// first frame arrives). QML uses it to size the item.
	Q_PROPERTY(qreal frameAspect READ frameAspect NOTIFY frameAspectChanged)

public:
	explicit VideoPreviewStreamItem(QQuickItem *parent = nullptr);
	~VideoPreviewStreamItem() override;

	QObject *source() const;
	void setSource(QObject *source);

	int previewId() const;
	void setPreviewId(int id);

	qreal frameAspect() const;

Q_SIGNALS:
	void sourceChanged();
	void previewIdChanged();
	void frameAspectChanged();

protected:
	QSGNode *updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data) override;
	void itemChange(ItemChange change, const ItemChangeData &value) override;

private:
	/// Scene-graph node that owns the imported dma-buf textures of this stream.
	class TextureCache;

	void handleFrameReady(int id);
	void refresh();
	void resetSceneGraph();
	void advanceFrame(const trikHal::FrameBufferPtr &buffer);
	void ensureVideoNode(QSGNode *root);
	void connectFrameSwapped();

	QObject *mSourceObject = nullptr;
	trikControl::VideoPreviewSource *mSource = nullptr;
	int mPreviewId = -1;
	qreal mFrameAspect = 4.0 / 3.0;

	QSGImageNode *mVideoNode = nullptr;
	QSGImageNode *mOverlayNode = nullptr;
	VideoOverlayTexture *mOverlayTexture = nullptr;
	TextureCache *mTextures = nullptr;

	quint64 mSequence = 0;
	quint32 mOverlayGeneration = 0;
	bool mOverlayInitialized = false;
	trikHal::FrameFormat mFormat;

	trikHal::FrameBufferPtr mCurrentBuffer;
	QVector<trikHal::FrameBufferPtr> mPendingRelease;

	QPointer<QQuickWindow> mWindow;
	bool mFrameSwappedConnected = false;
};

} // namespace trikGui
