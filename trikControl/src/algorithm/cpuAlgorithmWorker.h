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

#include <cstdint>
#include <memory>

#include <trikHal/frameFormat.h>

#include "videoAlgorithmWorker.h"

namespace trikControl {

/// Bytes per pixel in a BGRA8888 frame (B, G, R, X).
constexpr int bgraBytesPerPixel = 4;

/// A scaled BGRA frame handed to the algorithm.
struct BgraFrameView {
	/// First byte of the top-left pixel.
	uint8_t *data = nullptr;
	int width = 0;
	int height = 0;
	/// Bytes per row (may be larger than width * 4).
	int stride = 0;
};

/// CPU video-algorithm worker, built on VideoAlgorithmWorker.
///
/// Adds a raster algorithm on top of the shared capture/RGA/preview plumbing:
///
/// 1. handleFrame() asks scaledFormat() for the RGA destination;
/// 2. the RGA scales the camera frame into a pooled preview buffer, or into a
///    private scratch buffer when the display is busy/off;
/// 3. processFrame() runs the algorithm on the scaled BGRA frame;
/// 4. buildOverlay() optionally produces drawing commands for the preview;
/// 5. the frame is published to the preview sink.
///
/// A concrete sensor (color/line/object) only implements processFrame(),
/// buildOverlay(), and optionally scaledFormat()/modifiesOutputFrame().
///
/// @section threading Threading
///
/// processFrame() and buildOverlay() run on the worker thread; results that are
/// read from the control thread must be guarded by the concrete worker.
class CpuAlgorithmWorker : public VideoAlgorithmWorker
{
public:
	CpuAlgorithmWorker(QSharedPointer<CameraManager> camera, QString port,
		trikHal::HardwareAbstractionInterface &hal, std::shared_ptr<VideoPreviewSink> previewSink,
		int previewBufferCount = 4, QObject *parent = nullptr);
	~CpuAlgorithmWorker() override;

protected:
	/// Output format/size requested from the RGA for a source frame of the
	/// given geometry.
	///
	/// The default is VideoAlgorithmWorker::previewFormat() (clamped BGRA), so
	/// the algorithm buffer doubles as the preview buffer. Override it if the
	/// algorithm needs a different size/format; the result is then also used as
	/// the preview format by this worker.
	virtual trikHal::FrameFormat scaledFormat(int srcWidth, int srcHeight) const;

	/// Whether processFrame() may write into the frame buffer. When true the
	/// buffer is flushed for the display after processFrame().
	virtual bool modifiesOutputFrame() const
	{
		return false;
	}

	/// Runs the algorithm on the scaled BGRA frame.
	///
	/// Returning false skips the overlay build and the preview publish for this
	/// frame (the buffer is still synced and recycled).
	virtual bool processFrame(const BgraFrameView &frame) = 0;

	/// Builds the preview overlay after a successful processFrame().
	virtual OverlayResult buildOverlay(const BgraFrameView &frame) = 0;

	/// VideoAlgorithmWorker hook: scales/stages the frame and drives the
	/// algorithm/preview steps described in the class documentation.
	void handleFrame(Frame &frame) override;

private:
	/// Ensures the private fallback buffer can hold @p format.
	bool ensureScratch(const trikHal::FrameFormat &format);

	/// Scales and color-converts the staged camera frame into @p dst with the
	/// RGA and fills @p out with a view of the result.
	bool scaleWithRga(const Frame &src, int srcWidth, int srcHeight, int srcStride,
		trikKernel::PixelFormat srcFormat, const trikHal::FrameFormat &dstFormat, trikHal::FrameBuffer &dst,
		BgraFrameView *out);

	/// Private fallback buffer used when no pooled preview buffer is available.
	std::unique_ptr<trikHal::FrameBuffer> mScratch;
};

} // namespace trikControl
