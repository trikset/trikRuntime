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

#include "videoDmaTexture.h"

#include <drm/drm_fourcc.h>
#include <EGL/egl.h>
#include <EGL/eglext.h>

#include <QtGui/QOpenGLContext>
#include <QtGui/QOpenGLFunctions>
#include <QsLog.h>

using namespace trikGui;

// Importing a dma-buf as a GL texture is not part of core EGL/GLES. Core can
// only upload pixels from CPU memory (glTexImage2D); external memory needs
// extensions:
//
//   * EGL_EXT_image_dma_buf_import + EGL_KHR_image_base
//       -> eglCreateImageKHR(EGL_LINUX_DMA_BUF_EXT, ...)
//   * GL_OES_EGL_image
//       -> glEGLImageTargetTexture2DOES(GL_TEXTURE_2D, image)
//
// Extension entry points are not guaranteed to be exported by libEGL/libGLESv2,
// so their addresses are resolved at runtime with eglGetProcAddress. If they are
// missing, zero-copy import is impossible and the only fallback would be a CPU
// copy through glTexImage2D. The pointers are process-wide and cached after the
// first resolution, which happens on the scene graph render thread.
namespace {

using GlEglImageTargetTexture2DOES = void (*)(GLenum target, void *image);

PFNEGLCREATEIMAGEKHRPROC createImageKHR = nullptr;
PFNEGLDESTROYIMAGEKHRPROC destroyImageKHR = nullptr;
GlEglImageTargetTexture2DOES eglImageTargetTexture2DOES = nullptr;

void resolveEglFunctions()
{
	if (createImageKHR) {
		return;
	}

	createImageKHR = reinterpret_cast<PFNEGLCREATEIMAGEKHRPROC>(eglGetProcAddress("eglCreateImageKHR"));
	destroyImageKHR = reinterpret_cast<PFNEGLDESTROYIMAGEKHRPROC>(eglGetProcAddress("eglDestroyImageKHR"));
	eglImageTargetTexture2DOES =
		reinterpret_cast<GlEglImageTargetTexture2DOES>(eglGetProcAddress("glEGLImageTargetTexture2DOES"));
}

} // namespace

GlDmaTexture *GlDmaTexture::create(int fd, const trikHal::FrameFormat &format)
{
	QOpenGLContext *context = QOpenGLContext::currentContext();
	if (!context) {
		return nullptr;
	}

	resolveEglFunctions();
	if (!createImageKHR || !destroyImageKHR || !eglImageTargetTexture2DOES) {
		QLOG_ERROR() << "GlDmaTexture: EGL dma-buf import is not available";
		return nullptr;
	}

	EGLDisplay display = eglGetCurrentDisplay();
	if (display == EGL_NO_DISPLAY) {
		QLOG_ERROR() << "GlDmaTexture: no current EGL display";
		return nullptr;
	}

	const EGLint attributes[] = {EGL_WIDTH, static_cast<EGLint>(format.width), EGL_HEIGHT,
		static_cast<EGLint>(format.height), EGL_LINUX_DRM_FOURCC_EXT, DRM_FORMAT_XRGB8888,
		EGL_DMA_BUF_PLANE0_FD_EXT, fd, EGL_DMA_BUF_PLANE0_OFFSET_EXT, 0, EGL_DMA_BUF_PLANE0_PITCH_EXT,
		static_cast<EGLint>(format.stride), EGL_NONE};

	EGLImageKHR image = createImageKHR(display, EGL_NO_CONTEXT, EGL_LINUX_DMA_BUF_EXT, nullptr, attributes);
	if (image == EGL_NO_IMAGE_KHR) {
		QLOG_ERROR() << "GlDmaTexture: eglCreateImageKHR failed for fd" << fd << "error" << eglGetError();
		return nullptr;
	}

	auto *functions = context->functions();
	GLuint texture = 0;
	functions->glGenTextures(1, &texture);
	functions->glBindTexture(GL_TEXTURE_2D, texture);
	functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	functions->glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	eglImageTargetTexture2DOES(GL_TEXTURE_2D, image);

	const GLenum error = functions->glGetError();
	if (error != 0) {
		QLOG_ERROR() << "GlDmaTexture: glEGLImageTargetTexture2DOES failed, glError" << error;
		functions->glDeleteTextures(1, &texture);
		destroyImageKHR(display, image);
		return nullptr;
	}

	return new GlDmaTexture(texture, image, format);
}

GlDmaTexture::GlDmaTexture(GLuint texture, EGLImageKHR image, const trikHal::FrameFormat &format)
	: mTexture(texture)
	, mImage(image)
	, mSize(static_cast<int>(format.width), static_cast<int>(format.height))
{
}

GlDmaTexture::~GlDmaTexture()
{
	EGLDisplay display = eglGetCurrentDisplay();
	if (destroyImageKHR && display != EGL_NO_DISPLAY && mImage != EGL_NO_IMAGE_KHR) {
		destroyImageKHR(display, mImage);
	}

	if (mTexture && QOpenGLContext::currentContext()) {
		QOpenGLContext::currentContext()->functions()->glDeleteTextures(1, &mTexture);
	}
}

int GlDmaTexture::textureId() const
{
	return static_cast<int>(mTexture);
}

QSize GlDmaTexture::textureSize() const
{
	return mSize;
}

bool GlDmaTexture::hasAlphaChannel() const
{
	return false;
}

bool GlDmaTexture::hasMipmaps() const
{
	return false;
}

void GlDmaTexture::bind()
{
	if (QOpenGLContext::currentContext()) {
		QOpenGLContext::currentContext()->functions()->glBindTexture(GL_TEXTURE_2D, mTexture);
	}
}
