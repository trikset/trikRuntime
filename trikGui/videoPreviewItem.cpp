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

#include "videoPreviewItem.h"

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include <QtCore/QMetaObject>
#include <QtCore/QTimer>
#include <QtCore/QVariantList>
#include <QtCore/QtMath>
#include <QtQuick/QSGImageNode>
#include <QtQuick/QSGTexture>
#include <QtQuick/QQuickWindow>

#include <trikControl/videoPreviewSource.h>
#include <trikKernel/videoUtils.h>
#include "videoDmaTexture.h"
#include "videoOverlayTexture.h"

using namespace trikGui;

namespace {

/// Sweep all active sensors into one render pass at this rate.
constexpr int tickIntervalMs = 33;

/// Bootstrap preview size used until the first real frame arrives.
constexpr int previewWidth = trikKernel::videoPreviewWidth;
constexpr int previewHeight = trikKernel::videoPreviewHeight;

constexpr int slotSpacing = 4;

} // namespace

namespace trikGui {

/// Scene-graph node that owns the dma-buf textures of one preview entry.
///
/// The node is a child of the item's scene-graph root, so Qt deletes it on the
/// render thread when the root is destroyed. Its destruction therefore happens
/// with the GL/EGL context bound, which is where GlDmaTexture must be released.
class VideoPreviewOverlayItem::TextureCache : public QSGNode
{
public:
	/// Returns the cached texture for @p buffer, importing it on first use.
	///
	/// The cache is keyed by FrameBuffer::id() (unique and never reused) plus
	/// the format, because a pool may re-type the same buffer object. The
	/// pointer stays valid as long as this node is alive.
	GlDmaTexture *texture(const trikHal::FrameBuffer &buffer)
	{
		for (const auto &cached : mTextures) {
			if (cached.bufferId == buffer.id() && cached.format == buffer.format()) {
				return cached.texture.get();
			}
		}

		auto created = std::unique_ptr<GlDmaTexture>(GlDmaTexture::create(buffer.fd(), buffer.format()));
		if (!created) {
			return nullptr;
		}

		mTextures.emplace_back(buffer.id(), buffer.format(), std::move(created));
		return mTextures.back().texture.get();
	}

	/// Drops all imported textures. Must be called on the render thread with a
	/// current GL context, i.e. from updatePaintNode().
	void clear()
	{
		mTextures.clear();
	}

private:
	struct CachedTexture {
		CachedTexture(uint64_t bufferId, const trikHal::FrameFormat &format,
			std::unique_ptr<GlDmaTexture> texture)
			: bufferId(bufferId)
			, format(format)
			, texture(std::move(texture))
		{
		}

		uint64_t bufferId;
		trikHal::FrameFormat format;
		std::unique_ptr<GlDmaTexture> texture;
	};

	std::vector<CachedTexture> mTextures;
};

} // namespace trikGui

VideoPreviewOverlayItem::VideoPreviewOverlayItem(QQuickItem *parent)
	: QQuickItem(parent)
{
	setFlag(ItemHasContents, true);
	connect(this, &QQuickItem::widthChanged, this, &QQuickItem::update);

	mTicker = new QTimer(this);
	mTicker->setInterval(tickIntervalMs);
	mTicker->setTimerType(Qt::PreciseTimer);
	connect(mTicker, &QTimer::timeout, this, &VideoPreviewOverlayItem::tick);
	mTicker->start();
}

QObject *VideoPreviewOverlayItem::source() const
{
	return mSourceObject;
}

void VideoPreviewOverlayItem::setSource(QObject *source)
{
	if (mSourceObject == source) {
		return;
	}

	if (mSourceObject) {
		disconnect(mSourceObject, nullptr, this, nullptr);
	}

	mSourceObject = source;
	mSource = qobject_cast<trikControl::VideoPreviewSource *>(source);
	if (mSource) {
		connect(mSource, &trikControl::VideoPreviewSource::frameReady, this,
			[this](int) { mUpdatePending = true; });
		connect(mSource, &trikControl::VideoPreviewSource::activeChanged, this, [this]() {
			mUpdatePending = true;
			update();
		});
	}

	mUpdatePending = true;
	update();
	Q_EMIT sourceChanged();
}

int VideoPreviewOverlayItem::contentHeight() const
{
	return mContentHeight;
}

void VideoPreviewOverlayItem::updateContentHeight(int height)
{
	if (mPostedContentHeight == height) {
		return;
	}
	mPostedContentHeight = height;

	// mContentHeight belongs to the GUI thread, so apply the change there and
	// only then notify QML. The queued call is not posted for every frame,
	// only when the total height actually changes.
	QMetaObject::invokeMethod(this, [this, height]() {
		if (mContentHeight == height) {
			return;
		}
		mContentHeight = height;
		Q_EMIT contentHeightChanged();
	}, Qt::QueuedConnection);
}

void VideoPreviewOverlayItem::tick()
{
	if (!mUpdatePending) {
		return;
	}

	mUpdatePending = false;
	update();
}

void VideoPreviewOverlayItem::rebuildEntries(QSGNode *root, const QVector<int> &ids)
{
	mEntries.resize(ids.size());

	for (int index = 0; index < ids.size(); ++index) {
		auto &entry = mEntries[index];
		entry.id = ids[index];

		// Video and overlay nodes are created lazily from updateEntry(): the
		// image nodes must never be added without a texture, and video-only
		// clients do not need an overlay at all.
		entry.textures = new TextureCache;
		root->appendChildNode(entry.textures);
	}
}

int VideoPreviewOverlayItem::updateEntry(Entry &entry, QSGNode *root, qreal y)
{
	quint64 sequence = 0;
	const auto frame = mSource->latest(entry.id, &sequence);

	int frameWidth = previewWidth;
	int frameHeight = previewHeight;
	if (frame.buffer) {
		const auto &format = frame.buffer->format();
		frameWidth = static_cast<int>(format.width);
		frameHeight = static_cast<int>(format.height);
	}

	if (frameWidth <= 0 || frameHeight <= 0) {
		frameWidth = previewWidth;
		frameHeight = previewHeight;
	}

	const int availableWidth = width() > 0 ? qRound(width()) : previewWidth;
	const int displayWidth = qMin(frameWidth, availableWidth);
	const int height = qRound(static_cast<qreal>(displayWidth) * frameHeight / frameWidth);
	const qreal x = (width() - displayWidth) / 2.0;

	if (entry.overlayNode) {
		entry.overlayNode->setRect(x, y, displayWidth, height);
	}

	if (!frame.buffer) {
		return height;
	}

	const auto &format = frame.buffer->format();
	if (entry.format != format) {
		// The client changed resolution/stride/pixel format. Textures imported
		// for the old format cannot be reused, and a recycled pool may have
		// re-typed the same buffer objects.
		entry.textures->clear();
		entry.format = format;
	}

	auto *videoTexture = entry.textures->texture(*frame.buffer);
	if (!videoTexture) {
		return height;
	}

	advanceFrame(entry, frame.buffer);

	ensureVideoNode(entry, root);

	entry.videoNode->setRect(x, y, displayWidth, height);
	if (entry.videoNode->texture() != videoTexture) {
		entry.videoNode->setTexture(videoTexture);
	}

	if (sequence == entry.sequence) {
		return height;
	}

	entry.sequence = sequence;
	entry.videoNode->markDirty(QSGNode::DirtyMaterial);

	// A zero generation means the client never publishes overlays: do not
	// allocate a texture or a scene-graph node for it.
	if (frame.overlayGeneration == 0) {
		return height;
	}

	const auto size = QSize(frameWidth, frameHeight);

	if (!entry.overlayNode) {
		auto *overlayTexture = new VideoOverlayTexture;
		overlayTexture->setOverlay(frame.overlay, size);
		if (!overlayTexture->updateTexture()) {
			delete overlayTexture;
			return height;
		}

		entry.overlayTexture = overlayTexture;
		entry.overlayNode = window()->createImageNode();
		entry.overlayNode->setOwnsTexture(true);
		entry.overlayNode->setFiltering(QSGTexture::Linear);
		entry.overlayNode->setTexture(overlayTexture);
		entry.overlayNode->setRect(x, y, displayWidth, height);
		root->appendChildNode(entry.overlayNode);

		entry.overlayInitialized = true;
		entry.overlayGeneration = frame.overlayGeneration;
		return height;
	}

	if (!entry.overlayInitialized || entry.overlayGeneration != frame.overlayGeneration
		|| entry.overlayTexture->textureSize() != size) {
		entry.overlayInitialized = true;
		entry.overlayGeneration = frame.overlayGeneration;
		entry.overlayTexture->setOverlay(frame.overlay, size);
		if (entry.overlayTexture->updateTexture()) {
			entry.overlayNode->markDirty(QSGNode::DirtyMaterial);
		}
	}

	return height;
}

void VideoPreviewOverlayItem::advanceFrame(Entry &entry, const trikHal::FrameBufferPtr &buffer)
{
	if (entry.currentBuffer == buffer) {
		return;
	}

	if (entry.currentBuffer) {
		mPendingRelease.append(entry.currentBuffer);
	}

	entry.currentBuffer = buffer;
}

void VideoPreviewOverlayItem::ensureVideoNode(Entry &entry, QSGNode *root)
{
	if (entry.videoNode) {
		return;
	}

	// Do not put an untextured image node into the scene graph: it exposes an
	// opaque material and QSGOpaqueTextureMaterial::compare() would dereference
	// a null texture. Create the node only once its texture exists.
	entry.videoNode = window()->createImageNode();
	entry.videoNode->setFiltering(QSGTexture::Linear);

	if (entry.overlayNode) {
		auto *parent = entry.overlayNode->parent();
		parent->insertChildNodeBefore(entry.videoNode, entry.overlayNode);
	} else {
		root->appendChildNode(entry.videoNode);
	}
}

void VideoPreviewOverlayItem::retireCurrentBuffers(bool keepUntilSwap)
{
	if (!keepUntilSwap) {
		// The old tree is already gone, so nothing can still be sampling its
		// frames. Drop the current and previously retired buffers immediately.
		mPendingRelease.clear();
		return;
	}

	for (const auto &entry : mEntries) {
		if (entry.currentBuffer) {
			mPendingRelease.append(entry.currentBuffer);
		}
	}
}

QSGNode *VideoPreviewOverlayItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
	Q_UNUSED(data);

	if (!mSwappedConnected && window()) {
		mSwappedConnected = true;
		// frameSwapped is emitted by the render thread after the frame has been
		// presented, so buffers used by that frame are no longer read by the GPU.
		connect(window(), &QQuickWindow::frameSwapped, this, [this]() { mPendingRelease.clear(); },
			Qt::DirectConnection);
	}

	auto ids = QVector<int>();
	const auto activeIds = mSource ? mSource->activeIds() : QVariantList();
	ids.reserve(activeIds.size());
	for (const auto &id : activeIds) {
		ids.append(id.toInt());
	}

	if (!oldNode || ids != mIds) {
		// The item owns the whole subtree under oldNode: Qt only owns the
		// pointer it keeps as paintNode. Deleting it here (on the render
		// thread, with the GL context bound) also deletes every child and the
		// textures they own.
		retireCurrentBuffers(oldNode != nullptr);
		delete oldNode;
		oldNode = nullptr;

		mEntries.clear();
		mIds = ids;

		if (ids.isEmpty()) {
			updateContentHeight(0);
			return nullptr;
		}

		oldNode = new QSGNode;
		rebuildEntries(oldNode, ids);
	}

	qreal y = 0;
	int contentHeight = 0;
	for (auto &mEntrie : mEntries) {
		const int height = updateEntry(mEntrie, oldNode, y);
		y += height + slotSpacing;
		contentHeight += height;
	}

	if (!mEntries.isEmpty()) {
		contentHeight += slotSpacing * (mEntries.size() - 1);
	}
	updateContentHeight(contentHeight);

	return oldNode;
}
