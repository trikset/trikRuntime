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

#include <QtGui/QImage>
#include <QtGui/qopengl.h>
#include <QtQuick/QSGDynamicTexture>

#include <trikControl/videoPreviewTypes.h>

namespace trikGui {

/// Overlay layer composited on top of the camera frame in the preview.
///
/// The standard pipeline is: a video sensor captures a frame, processes it
/// (for example, builds a color grid), and describes annotations for that
/// frame as PreviewOverlay paths. This texture turns those paths into an RGBA
/// layer that the scene graph draws over the zero-copy video texture.
///
/// The current implementation rasterizes the QPainterPath list on the CPU with
/// QPainter into a QImage and uploads it to GL. This keeps arbitrary Qt paths
/// available without a GPU tessellator, but costs a CPU raster pass plus a
/// texture upload. A GPU geometry path (for example, axis-aligned rectangles
/// as quads) is a possible future optimization.
///
/// The class derives from QSGDynamicTexture because its contents change over
/// time. hasAlphaChannel() returns true, so Qt composites it with blending over
/// the video instead of drawing it as an opaque layer.
///
/// Like every QSG class, this object belongs to the scene graph render thread:
/// setOverlay() and updateTexture() are normally called from
/// QQuickItem::updatePaintNode().
class VideoOverlayTexture : public QSGDynamicTexture
{
public:
	/// Releases the GL texture if a GL context is current.
	///
	/// Without a current context the object is destroyed without GL calls; the
	/// context cleanup then owns the underlying resource.
	~VideoOverlayTexture() override;

	/// Rasterizes @p overlay into the internal RGBA image of @p size.
	///
	/// @p size is the frame size in pixels, not the on-screen item size; the
	/// QSGImageNode scales the texture to its rect. The image is cleared to
	/// transparent first, so @p overlay describes the whole overlay rather than
	/// an incremental update. Coordinates in @p overlay are frame pixels.
	///
	/// This only touches CPU memory and sets the dirty flag; the GL upload is
	/// performed by updateTexture(). Must not run concurrently with
	/// updateTexture() on the same object.
	void setOverlay(const trikControl::PreviewOverlay &overlay, const QSize &size);

	/// Uploads the pending overlay image to the GL texture.
	///
	/// Requires a current GL context, i.e. the scene graph render thread.
	/// Allocates or reallocates storage with glTexImage2D when the image size
	/// differs from the size already uploaded, otherwise updates the pixels with
	/// glTexSubImage2D.
	///
	/// @return true when an upload happened; false when nothing was dirty or no
	/// GL context is current. QSGDynamicTexture consumers use the return value
	/// to decide whether the material must be marked dirty.
	bool updateTexture() override;

	/// Binds the GL texture to the current texture unit.
	///
	/// Called by the scene graph before drawing. Does nothing when no GL context
	/// is current.
	void bind() override;

	/// GL name of the texture, or 0 before the first successful upload.
	int textureId() const override;

	/// Size of the staged overlay image.
	///
	/// Returns 1x1 while no image has been allocated so that QSG geometry never
	/// receives a zero size (a zero texture size makes QSGImageNode produce NaN
	/// texture coordinates).
	QSize textureSize() const override;

	/// Always true: the overlay is transparent except where paths are drawn, so
	/// Qt must composite it over the video with alpha blending.
	bool hasAlphaChannel() const override;

	/// Always false: only one mip level is uploaded.
	bool hasMipmaps() const override;

private:
	/// GL texture name; 0 until the first successful updateTexture().
	GLuint mTexture = 0;

	/// CPU staging image in Format_RGBA8888_Premultiplied, i.e. the format Qt
	/// Quick expects for premultiplied-alpha blending.
	QImage mImage;

	/// Size currently allocated in the GL texture. Used to choose between
	/// glTexImage2D and glTexSubImage2D; an empty size forces reallocation.
	QSize mUploadedSize;

	/// Set by setOverlay(), cleared by updateTexture().
	bool mDirty = false;
};

} // namespace trikGui
