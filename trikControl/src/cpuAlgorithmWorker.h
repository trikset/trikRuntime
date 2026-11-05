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

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>

#include <QtCore/QSharedPointer>
#include <QtCore/QString>
#include <QtCore/QThread>

#include <trikControl/videoPreviewSink.h>
#include <trikControl/videoPreviewTypes.h>
#include <trikHal/dmaHeapInterface.h>
#include <trikHal/frameBuffer.h>
#include <trikHal/frameFormat.h>
#include <trikHal/hardwareAbstractionInterface.h>
#include <trikHal/rgaInterface.h>
#include <trikKernel/videoUtils.h>

#include "cameraManager.h"

namespace trikControl {

/// A scaled BGRA frame handed to the algorithm.
struct BgraFrameView {
	const uint8_t *data = nullptr;
	int width = 0;
	int height = 0;
	int stride = 0;
};

/// Overlay produced by an algorithm after processing a frame.
struct OverlayResult {
	PreviewOverlay overlay;
	quint32 generation = 0;
};

/// Common CPU video-algorithm worker.
///
/// Owns the capture loop, the RGA scaler, the DMA input/scratch buffers and the
/// preview publishing. A concrete algorithm only provides the target format,
/// processes the scaled BGRA frame and builds its overlay.
class CpuAlgorithmWorker : public QThread
{
public:
	CpuAlgorithmWorker(QSharedPointer<CameraManager> camera, int portId, QString port,
		trikHal::HardwareAbstractionInterface &hal, VideoPreviewSink *previewSink,
		int previewBufferCount = 4, QObject *parent = nullptr);
	~CpuAlgorithmWorker() override;

	/// Acquires the camera, activates preview when @p showOnDisplay is true and
	/// starts the capture loop.
	void start(bool showOnDisplay);

	/// Stops the loop and deactivates preview. Releases the camera when
	/// @p releaseCamera is true.
	void stop(bool releaseCamera = true);

protected:
	void run() override;

	/// Output format/size requested from the RGA for a source frame of the
	/// given geometry.
	virtual trikHal::FrameFormat scaledFormat(int srcWidth, int srcHeight) const = 0;

	/// Runs the algorithm on the scaled BGRA frame. Returning false skips the
	/// overlay and preview publishing for this frame.
	virtual bool processFrame(const BgraFrameView &frame) = 0;

	/// Builds the overlay after a successful processFrame().
	virtual OverlayResult buildOverlay(const BgraFrameView &frame) = 0;

private:
	void processSourceFrame(Frame &frame);
	bool ensureInputBuffer(size_t bytes);
	bool ensureScratch(const trikHal::FrameFormat &format);
	bool scaleWithRga(const Frame &src, int srcWidth, int srcHeight, int srcStride,
		trikKernel::PixelFormat srcFormat, const trikHal::FrameFormat &dstFormat,
		trikHal::FrameBuffer &dst, BgraFrameView *out);

	QSharedPointer<CameraManager> mCamera;
	int mPortId = -1;
	QString mPort;

	trikHal::HardwareAbstractionInterface &mHal;
	VideoPreviewSink *mSink = nullptr;

	std::unique_ptr<trikHal::RgaInterface> mRga;
	std::unique_ptr<trikHal::DmaHeapInterface> mDmaHeap;
	std::unique_ptr<trikHal::DmaBufferInterface> mScaleInput;
	std::unique_ptr<trikHal::FrameBuffer> mScratch;

	int mPreviewBufferCount = 3;
	bool mPreviewEnabled = false;
	bool mCameraAcquired = false;
	bool mAvailable = false;
	bool mRgaErrorLogged = false;
	bool mGeometryWarned = false;

	std::atomic<bool> mRunning {false};
	std::atomic<bool> mAbort {false};
};

} // namespace trikControl
