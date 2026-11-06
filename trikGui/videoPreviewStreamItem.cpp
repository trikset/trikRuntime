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

#include "videoPreviewStreamItem.h"

#include <cstdint>
#include <memory>
#include <utility>
#include <vector>

#include <QtCore/QMetaObject>
#include <QtCore/QSize>
#include <QtCore/QtGlobal>
#include <QtQuick/QSGImageNode>
#include <QtQuick/QSGTexture>
#include <QtQuick/QQuickWindow>

#include <trikControl/videoPreviewSource.h>
#include <trikKernel/videoUtils.h>

#include "videoDmaTexture.h"
#include "videoOverlayTexture.h"

using namespace trikGui;

namespace trikGui {

/// Scene-graph node that owns the dma-buf textures of one stream.
///
/// The node is a child of the item's scene-graph root, so Qt deletes it on the
/// render thread when the root is destroyed. Its destruction therefore happens
/// with the GL/EGL context bound, which is where GlDmaTexture must be released.
class VideoPreviewStreamItem::TextureCache : public QSGNode
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

VideoPreviewStreamItem::VideoPreviewStreamItem(QQuickItem *parent)
	: QQuickItem(parent)
{
	setFlag(ItemHasContents, true);
	connect(this, &QQuickItem::widthChanged, this, &QQuickItem::update);
	connect(this, &QQuickItem::heightChanged, this, &QQuickItem::update);

	mFrameAspect = static_cast<qreal>(trikKernel::videoPreviewWidth) / trikKernel::videoPreviewHeight;
}

VideoPreviewStreamItem::~VideoPreviewStreamItem()
{
	auto retired = std::move(mPendingRelease);
	if (mCurrentBuffer) {
		retired.append(std::move(mCurrentBuffer));
	}

	auto *window = mWindow.data();
	if (retired.isEmpty() || !window) {
		return;
	}

	// Keep the last frames alive until the frame that may still sample them has
	// been presented. The connection removes itself on frameSwapped, which also
	// releases the captured frames on the render thread.
	auto frames = std::make_shared<QVector<trikHal::FrameBufferPtr>>(std::move(retired));
	auto connection = std::make_shared<QMetaObject::Connection>();
	*connection = connect(window, &QQuickWindow::frameSwapped, window, [frames, connection]() {
		Q_UNUSED(frames);
		QObject::disconnect(*connection);
	}, Qt::DirectConnection);
}

QObject *VideoPreviewStreamItem::source() const
{
	return mSourceObject;
}

void VideoPreviewStreamItem::setSource(QObject *source)
{
	if (mSourceObject == source) {
		return;
	}

	if (mSource) {
		disconnect(mSource, &trikControl::VideoPreviewSource::frameReady, this,
			&VideoPreviewStreamItem::handleFrameReady);
	}

	mSourceObject = source;
	mSource = qobject_cast<trikControl::VideoPreviewSource *>(source);
	if (mSource) {
		connect(mSource, &trikControl::VideoPreviewSource::frameReady, this,
			&VideoPreviewStreamItem::handleFrameReady);
	}

	refresh();
	Q_EMIT sourceChanged();
}

int VideoPreviewStreamItem::previewId() const
{
	return mPreviewId;
}

void VideoPreviewStreamItem::setPreviewId(int id)
{
	if (mPreviewId == id) {
		return;
	}

	mPreviewId = id;
	refresh();
	Q_EMIT previewIdChanged();
}

qreal VideoPreviewStreamItem::frameAspect() const
{
	return mFrameAspect;
}

void VideoPreviewStreamItem::itemChange(ItemChange change, const ItemChangeData &value)
{
	if (change == ItemSceneChange && value.window) {
		mWindow = value.window;
	}

	QQuickItem::itemChange(change, value);
}

void VideoPreviewStreamItem::handleFrameReady(int id)
{
	if (id == mPreviewId) {
		refresh();
	}
}

void VideoPreviewStreamItem::refresh()
{
	if (!mSource || mPreviewId < 0) {
		update();
		return;
	}

	auto sequence = quint64 {0};
	const auto frame = mSource->latest(mPreviewId, &sequence);
	if (frame.buffer) {
		const auto &format = frame.buffer->format();
		if (format.width > 0 && format.height > 0) {
			const auto aspect = static_cast<qreal>(format.width) / format.height;
			if (!qFuzzyCompare(aspect, mFrameAspect)) {
				mFrameAspect = aspect;
				Q_EMIT frameAspectChanged();
			}
		}
	}

	update();
}

void VideoPreviewStreamItem::resetSceneGraph()
{
	mTextures = nullptr;
	mVideoNode = nullptr;
	mOverlayNode = nullptr;
	mOverlayTexture = nullptr;
	mOverlayInitialized = false;
	mOverlayGeneration = 0;
	mSequence = 0;
	mFormat = {};
}

void VideoPreviewStreamItem::connectFrameSwapped()
{
	if (mFrameSwappedConnected || !mWindow) {
		return;
	}

	mFrameSwappedConnected = true;
	connect(mWindow.data(), &QQuickWindow::frameSwapped, this, [this]() { mPendingRelease.clear(); },
		Qt::DirectConnection);
}

void VideoPreviewStreamItem::advanceFrame(const trikHal::FrameBufferPtr &buffer)
{
	if (mCurrentBuffer == buffer) {
		return;
	}

	if (mCurrentBuffer) {
		mPendingRelease.append(mCurrentBuffer);
	}

	mCurrentBuffer = buffer;
}

void VideoPreviewStreamItem::ensureVideoNode(QSGNode *root)
{
	if (mVideoNode) {
		return;
	}

	// Do not put an untextured image node into the scene graph: it exposes an
	// opaque material and QSGOpaqueTextureMaterial::compare() would dereference
	// a null texture. Create the node only once its texture exists.
	mVideoNode = window()->createImageNode();
	mVideoNode->setFiltering(QSGTexture::Linear);

	if (mOverlayNode) {
		auto *parent = mOverlayNode->parent();
		parent->insertChildNodeBefore(mVideoNode, mOverlayNode);
	} else {
		root->appendChildNode(mVideoNode);
	}
}

QSGNode *VideoPreviewStreamItem::updatePaintNode(QSGNode *oldNode, UpdatePaintNodeData *data)
{
	Q_UNUSED(data);

	connectFrameSwapped();

	if (!mSource || mPreviewId < 0 || !window()) {
		delete oldNode;
		resetSceneGraph();
		return nullptr;
	}

	auto *root = oldNode;
	if (!root) {
		root = new QSGNode;
		mTextures = new TextureCache;
		root->appendChildNode(mTextures);
		mVideoNode = nullptr;
		mOverlayNode = nullptr;
		mOverlayTexture = nullptr;
		mOverlayInitialized = false;
		mOverlayGeneration = 0;
		mSequence = 0;
		mFormat = {};
	}

	auto sequence = quint64 {0};
	const auto frame = mSource->latest(mPreviewId, &sequence);
	if (!frame.buffer) {
		return root;
	}

	const auto &format = frame.buffer->format();
	const auto frameWidth = static_cast<int>(format.width);
	const auto frameHeight = static_cast<int>(format.height);
	if (frameWidth <= 0 || frameHeight <= 0 || width() <= 0 || height() <= 0) {
		return root;
	}

	if (mFormat != format) {
		// The client changed resolution/stride/pixel format. Textures imported
		// for the old format cannot be reused, and a recycled pool may have
		// re-typed the same buffer objects.
		mTextures->clear();
		mFormat = format;
	}

	auto *videoTexture = mTextures->texture(*frame.buffer);
	if (!videoTexture) {
		return root;
	}

	advanceFrame(frame.buffer);
	ensureVideoNode(root);

	const auto itemWidth = width();
	const auto itemHeight = height();
	auto displayWidth = itemWidth;
	auto displayHeight = displayWidth * frameHeight / frameWidth;
	if (displayHeight > itemHeight) {
		displayHeight = itemHeight;
		displayWidth = displayHeight * frameWidth / frameHeight;
	}

	const auto x = (itemWidth - displayWidth) / 2.0;
	const auto y = qreal {0.0};

	mVideoNode->setRect(x, y, displayWidth, displayHeight);
	if (mVideoNode->texture() != videoTexture) {
		mVideoNode->setTexture(videoTexture);
	}

	if (sequence == mSequence) {
		return root;
	}

	mSequence = sequence;
	mVideoNode->markDirty(QSGNode::DirtyMaterial);

	// A zero generation means the client never publishes overlays: do not
	// allocate a texture or a scene-graph node for it.
	if (frame.overlayGeneration == 0) {
		return root;
	}

	const auto size = QSize(frameWidth, frameHeight);

	if (!mOverlayNode) {
		auto *overlayTexture = new VideoOverlayTexture;
		overlayTexture->setOverlay(frame.overlay, size);
		if (!overlayTexture->updateTexture()) {
			delete overlayTexture;
			return root;
		}

		mOverlayTexture = overlayTexture;
		mOverlayNode = window()->createImageNode();
		mOverlayNode->setOwnsTexture(true);
		mOverlayNode->setFiltering(QSGTexture::Linear);
		mOverlayNode->setTexture(overlayTexture);
		mOverlayNode->setRect(x, y, displayWidth, displayHeight);
		root->appendChildNode(mOverlayNode);

		mOverlayInitialized = true;
		mOverlayGeneration = frame.overlayGeneration;
		return root;
	}

	mOverlayNode->setRect(x, y, displayWidth, displayHeight);

	if (!mOverlayInitialized || mOverlayGeneration != frame.overlayGeneration
		|| mOverlayTexture->textureSize() != size) {
		mOverlayInitialized = true;
		mOverlayGeneration = frame.overlayGeneration;
		mOverlayTexture->setOverlay(frame.overlay, size);
		if (mOverlayTexture->updateTexture()) {
			mOverlayNode->markDirty(QSGNode::DirtyMaterial);
		}
	}

	return root;
}
