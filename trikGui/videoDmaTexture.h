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

#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <QtGui/qopengl.h>
#include <QtQuick/QSGTexture>

#include <trikHal/frameFormat.h>

namespace trikGui {

/// QSGTexture backed by a dma-buf imported through EGLImage.
///
/// No pixel upload: a producer (for example, RGA) writes the dma-buf and the
/// GPU samples the same pages.
/// Import uses the EGL_EXT_image_dma_buf_import and GL_OES_EGL_image
/// extensions, resolved at runtime with eglGetProcAddress.
///
/// Render thread only, with the Qt GL/EGL context current. The object owns the
/// EGLImage and GL texture, not the dma-buf or its fd.
class GlDmaTexture : public QSGTexture
{
public:
	/// Imports @p fd as XRGB8888 (BGRA memory layout, alpha ignored) and returns
	/// a texture owned by the caller.
	///
	/// The caller must keep the FrameBuffer alive while the texture is used.
	/// Returns nullptr if the extensions are unavailable or the import fails.
	static GlDmaTexture *create(int fd, const trikHal::FrameFormat &format);

	/// Deletes the EGLImage and GL texture when a context is current.
	~GlDmaTexture() override;

	int textureId() const override; ///< GL texture name.
	QSize textureSize() const override; ///< FrameFormat size.
	bool hasAlphaChannel() const override; ///< Always false (XRGB8888).
	bool hasMipmaps() const override; ///< Always false.
	void bind() override; ///< Binds the texture to the current texture unit.

private:
	GlDmaTexture(GLuint texture, EGLImageKHR image, const trikHal::FrameFormat &format);

	GLuint mTexture = 0; ///< GL texture name.
	EGLImageKHR mImage = EGL_NO_IMAGE_KHR; ///< Owned EGLImage.
	QSize mSize; ///< Cached image size.
};

} // namespace trikGui
