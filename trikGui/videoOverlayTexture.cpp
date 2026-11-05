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

#include "videoOverlayTexture.h"

#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QtGui/QPainter>

using namespace trikGui;

VideoOverlayTexture::~VideoOverlayTexture()
{
	// The scene graph usually destroys textures while its context is current.
	// Without a current context the GL object is left to the context cleanup.
	if (mTexture && QOpenGLContext::currentContext()) {
		QOpenGLContext::currentContext()->functions()->glDeleteTextures(1, &mTexture);
		mTexture = 0;
	}
}

void VideoOverlayTexture::setOverlay(const trikControl::PreviewOverlay &overlay, const QSize &size)
{
	// @p size is the frame size in pixels (for example 320x240), not the size of
	// the on-screen item; QSGImageNode scales the texture to its own rect.
	if (mImage.size() != size) {
		mImage = QImage(size, QImage::Format_RGBA8888_Premultiplied);
		// The GL storage still has the previous size: force glTexImage2D on the
		// next updateTexture().
		mUploadedSize = QSize();
	}

	// Every call describes the complete overlay. Start from a fully transparent
	// layer so paths removed by the algorithm do not leave stale pixels behind.
	mImage.fill(Qt::transparent);
	if (!overlay.isEmpty()) {
		QPainter painter(&mImage);
		// Overlays are normally axis-aligned rectangles; antialiasing is not
		// needed and costs extra CPU time.
		painter.setRenderHint(QPainter::Antialiasing, false);
		for (const auto &item : overlay) {
			painter.setPen(item.pen);
			painter.setBrush(item.brush);
			painter.drawPath(item.path);
		}
	}

	mDirty = true;
}

bool VideoOverlayTexture::updateTexture()
{
	if (!mDirty || mImage.isNull()) {
		return false;
	}

	QOpenGLContext *context = QOpenGLContext::currentContext();
	if (!context) {
		// Scene graph render thread only; retry on the next call.
		return false;
	}

	auto *functions = context->functions();

	// Create and configure the texture object once. Its filtering/wrapping state
	// persists, so later updates only need to bind it again.
	if (!mTexture) {
		functions->glGenTextures(1, &mTexture);
		functions->glBindTexture(GL_TEXTURE_2D, mTexture);
		functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	} else {
		functions->glBindTexture(GL_TEXTURE_2D, mTexture);
	}

	// Format_RGBA8888_Premultiplied has R,G,B,A bytes and 4-byte aligned rows,
	// so it can be uploaded as GL_RGBA/GL_UNSIGNED_BYTE without conversion.
	functions->glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	if (mUploadedSize != mImage.size()) {
		// First upload or frame size changed: (re)allocate the texture storage.
		functions->glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, mImage.width(), mImage.height(), 0, GL_RGBA,
			GL_UNSIGNED_BYTE, mImage.constBits());
		mUploadedSize = mImage.size();
	} else {
		// Same size: only replace the texels.
		functions->glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, mImage.width(), mImage.height(), GL_RGBA,
			GL_UNSIGNED_BYTE, mImage.constBits());
	}

	mDirty = false;
	return true;
}

void VideoOverlayTexture::bind()
{
	// QSG makes the render context current before binding materials.
	if (QOpenGLContext::currentContext()) {
		QOpenGLContext::currentContext()->functions()->glBindTexture(GL_TEXTURE_2D, mTexture);
	}
}

int VideoOverlayTexture::textureId() const
{
	// GL texture name, or 0 before the first successful upload.
	return static_cast<int>(mTexture);
}

QSize VideoOverlayTexture::textureSize() const
{
	// A zero texture size makes QSGImageNode produce NaN texture coordinates;
	// report a harmless size until the first overlay image exists.
	return mImage.isNull() ? QSize(1, 1) : mImage.size();
}

bool VideoOverlayTexture::hasAlphaChannel() const
{
	// The overlay is transparent except where paths are drawn, so Qt must
	// composite it over the video with alpha blending.
	return true;
}

bool VideoOverlayTexture::hasMipmaps() const
{
	// Only one mip level is uploaded.
	return false;
}
