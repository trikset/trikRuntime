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
#include <QtGui/QBrush>
#include <QtGui/QPainterPath>
#include <QtGui/QPen>

#include <trikHal/frameBuffer.h>

namespace trikControl {

/// One drawing command of a preview overlay, in frame pixel coordinates.
struct PreviewOverlayPath {
	QPainterPath path;
	QPen pen = Qt::NoPen;
	QBrush brush = Qt::NoBrush;
};

using PreviewOverlay = QVector<PreviewOverlayPath>;

/// One display frame produced by a video sensor.
struct VideoPreviewFrame {
	trikHal::FrameBufferPtr buffer;
	PreviewOverlay overlay;
	/// Incremented by the producer whenever @c overlay changes. Consumers use
	/// it to avoid re-rasterizing/re-uploading an unchanged overlay.
	///
	/// Zero means the producer never publishes an overlay (video-only client):
	/// the consumer then does not create an overlay layer at all. A producer
	/// that starts with an overlay must use a non-zero generation.
	quint32 overlayGeneration = 0;
};

} // namespace trikControl
