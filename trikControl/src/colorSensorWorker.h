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

#include <QtCore/QMutex>
#include <QtCore/QSharedPointer>
#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtCore/QVector>
#include <atomic>
#include <cstddef>
#include <cstdint>
#include <memory>

#include <trikHal/dmaHeapInterface.h>
#include <trikHal/framePool.h>
#include <trikHal/rgaInterface.h>
#include <trikKernel/videoUtils.h>

#include <trikControl/videoPreviewSink.h>

#include "cameraManager.h"

namespace trikControl {

/// CPU implementation of the MxN color grid, bit-compatible with the old DSP
/// Mxn algorithm.
///
/// Pulls the newest frame from CameraManager and computes the dominant HSV
/// color of every grid cell using exactly the DSP integer math: the same
/// BT.601 YUV->RGB coefficients, the same fixed-point RGB->HSV conversion and
/// the same "first bin to reach the maximum wins" tie-breaking rule.
///
/// The DSP always worked on a 320x240 frame, so the capture is scaled to that
/// geometry first (cells then match the DSP). Every frame is routed through the
/// RGA, which performs the scale and YUV->RGB conversion in one job; the CPU
/// then only computes the HSV histogram. There is no CPU fallback: if the RGA
/// is unavailable or fails, an error is logged and the frame is skipped.
class ColorSensorWorker : public QThread
{
public:
	ColorSensorWorker(QSharedPointer<CameraManager> camera, int portId, QString port, int m, int n,
		bool showOnDisplay, trikHal::RgaInterface *rga, trikHal::DmaHeapInterface *dmaHeap,
		trikHal::FramePool *previewPool, VideoPreviewSink *previewSink, QObject *parent = nullptr);
	~ColorSensorWorker() override;

	/// Stops the loop. Safe to call from another thread; wakes a pending
	/// CameraManager::getFrame() wait.
	void stop();

	/// Returns the last RGB value of cell (@p mIndex, @p nIndex), 1-based.
	/// Returns {-1,-1,-1} for an invalid index.
	QVector<int> cell(int mIndex, int nIndex) const;

protected:
	void run() override;

private:
	/// Fixed 3x3 grid of RGB triplets (the DSP supports at most 3x3 cells).
	struct Grid {
		int colors[3][3][3] = {};
	};

	void processFrame(Frame &frame);
	Grid buildGridFromBgra(const uint8_t *bgra, int width, int height, int stride) const;

	/// Builds algorithm-agnostic drawing commands (colored cells) for the
	/// display, in frame pixel coordinates.
	PreviewOverlay buildOverlay(const Grid &grid, int width, int height) const;

	/// Scales and converts a frame to BGRA8888 with the RGA in one job.
	/// Returns the packed BGRA buffer and its byte stride, or false when the RGA
	/// is unavailable/failed (the caller logs the error and skips the frame).
	bool scaleWithRga(const Frame &frame, int srcWidth, int srcHeight, int srcStride,
		trikKernel::PixelFormat format, int targetWidth, int targetHeight,
		trikHal::FrameBuffer &dst, const uint8_t **outData, int *outStride);

	QSharedPointer<CameraManager> mCamera;
	int mPortId;
	QString mPort;
	int mM;
	int mN;
	bool mShowOnDisplay = false;

	trikHal::RgaInterface *mRga = nullptr;
	trikHal::DmaHeapInterface *mDmaHeap = nullptr;
	trikHal::FramePool *mPreviewPool = nullptr;
	VideoPreviewSink *mPreviewSink = nullptr;
	std::unique_ptr<trikHal::DmaBufferInterface> mScaleInput;
	std::unique_ptr<trikHal::FrameBuffer> mScaleScratch;
	bool mRgaWarned = false;

	std::atomic<bool> mRunning {true};
	std::atomic<bool> mAbort {false};

	mutable QMutex mGridLock;
	Grid mGrid;
	Grid mLastOverlayGrid;
	bool mOverlayGridValid = false;
	PreviewOverlay mLastOverlay;
	quint32 mOverlayGeneration = 0;
	bool mFormatWarned = false;
	bool mSizeWarned = false;
};

} // namespace trikControl
