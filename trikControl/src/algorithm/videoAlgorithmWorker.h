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
#include <memory>

#include <QtCore/QSharedPointer>
#include <QtCore/QString>
#include <QtCore/QThread>

#include <trikControl/videoPreviewSink.h>
#include <trikControl/videoPreviewTypes.h>
#include <trikControl/videoSensorStopFlags.h>
#include <trikHal/dmaHeapInterface.h>
#include <trikHal/frameBuffer.h>
#include <trikHal/frameFormat.h>
#include <trikHal/hardwareAbstractionInterface.h>
#include <trikHal/rgaInterface.h>
#include <trikKernel/videoUtils.h>

#include "cameraManager.h"

namespace trikControl {

/// Overlay produced by an algorithm after processing a frame.
struct OverlayResult {
	/// Drawing commands in preview-frame pixel coordinates.
	PreviewOverlay overlay;
	/// Incremented whenever @c overlay changes; 0 means "no overlay".
	quint32 generation = 0;
};

/// Common plumbing for camera-based video-algorithm workers.
///
/// This class owns everything a video sensor needs except the algorithm itself:
///
/// - the camera lifetime (`CameraManager::acquire()` / `release()` /
///   `stopStreaming()` / `abortFrameWaits()`);
/// - the worker thread and the capture loop (`start()` / `stop()` / `run()`);
/// - the RGA scaler and a staging dma-buf that holds the camera frame as an
///   RGA-readable input (`stageSource()`);
/// - the preview sink lifetime and publishing (`previewOn()`,
///   `acquirePreviewBuffer*()`, `publishPreview()`).
///
/// A concrete worker only implements handleFrame(): it decides what the RGA
/// should produce and how to process the frame. The base never hardcodes a
/// resolution or a pixel format for the algorithm.
///
/// @section threading Threading
///
/// `start()` / `stop()` are called from the control thread; `handleFrame()` runs
/// on the worker thread. Shared state that readers poll (algorithm results) must
/// be guarded by the concrete worker. `mPreviewId` and the camera handle are
/// only touched by start()/stop() (control thread) and by the base.
///
/// @section preview Preview
///
/// The preview sink is optional: when `start(false)` is used no preview is
/// registered and `previewOn()` stays false, so derived workers skip all
/// preview work. When the preview is on, the derived worker acquires a pooled
/// BGRA frame, fills it (usually with the RGA) and calls publishPreview().
class VideoAlgorithmWorker : public QThread
{
public:
	/// @param camera shared camera manager (may be null for a headless worker);
	/// @param port camera port name (e.g. "usb-camera-1");
	/// @param hal hardware abstraction used to create RGA/DMA/preview backends;
	/// @param previewSink optional display sink; when null no preview can be shown;
	/// @param previewBufferCount number of pooled preview buffers to request.
	VideoAlgorithmWorker(QSharedPointer<CameraManager> camera, QString port,
		trikHal::HardwareAbstractionInterface &hal, std::shared_ptr<VideoPreviewSink> previewSink,
		int previewBufferCount = 4, QObject *parent = nullptr);
	~VideoAlgorithmWorker() override;

	/// Acquires the camera, activates the preview when @p showOnDisplay is true
	/// and starts the capture loop.
	///
	/// Does nothing until isReadyToStart() returns true and a camera manager is
	/// present. A second call while the loop is running is ignored.
	void start(bool showOnDisplay);

	/// Stops the capture loop and deactivates the preview.
	///
	/// @param flags StopNone keeps the camera streaming and acquired;
	///              StopStream stops streaming but keeps the device open for a
	///              quick re-acquire; StopAll (default) releases the camera.
	void stop(int flags = StopAll);

protected:
	/// True when the worker has everything it needs to start. The default is
	/// "RGA and DMA heap are available". Subclasses with extra prerequisites
	/// (for example a loaded model) override this.
	virtual bool isReadyToStart() const;

	/// Processes one captured frame. Called on the worker thread for every valid
	/// frame. The implementation must call @c frame.release() when done with the
	/// camera buffer (the base does not release it for the worker).
	virtual void handleFrame(Frame &frame) = 0;

	/// Output format/size a preview frame should have for a source geometry.
	///
	/// The default clamps the source size to the display size and uses BGRA
	/// (`trikHal::PixelFormat::Bgra8888`) with a tightly packed stride.
	virtual trikHal::FrameFormat previewFormat(int srcWidth, int srcHeight) const;

	/// Copies the camera frame into the staging dma-buf used as the RGA source.
	///
	/// The camera buffer is not guaranteed to be a dma-buf, so it is copied into
	/// `mScaleInput`, which is then handed to the RGA. On success
	/// @p srcStridePixels receives the source row stride in pixels (for YUYV the
	/// byte stride is halved).
	bool stageSource(const Frame &frame, uint32_t srcStride, uint32_t srcHeight, trikKernel::PixelFormat srcFormat,
		int *srcStridePixels);

	/// Grows the staging buffer to at least @p bytes if needed.
	bool ensureInputBuffer(std::size_t bytes);

	/// Maps a camera pixel format to the RGA format. Returns
	/// `RgaFormat::Unknown` when the RGA cannot consume the format.
	static trikHal::RgaFormat rgaFormat(trikKernel::PixelFormat format);

	/// True when a preview is active and frames can be published.
	bool previewOn() const;

	/// Acquires a writable pooled preview frame of @p format, or nullptr when
	/// preview is off or every pooled buffer is currently held by a consumer.
	trikHal::FrameBufferPtr acquirePreviewBuffer(const trikHal::FrameFormat &format);

	/// Same as acquirePreviewBuffer() but derives the format from a source
	/// geometry via previewFormat().
	trikHal::FrameBufferPtr acquirePreviewBufferFor(int srcWidth, int srcHeight);

	/// Publishes @p buffer as the newest preview frame with @p overlay.
	///
	/// The sink is latest-wins, so a slow display consumer only loses
	/// intermediate frames. No-op when preview is off.
	void publishPreview(trikHal::FrameBufferPtr buffer, const OverlayResult &overlay);

	void run() override;

	/// Camera manager shared with the camera device; may be null.
	QSharedPointer<CameraManager> mCamera;
	/// Camera port id resolved from mPort; -1 when the port is unknown.
	const int mPortId = -1;
	/// Camera port name.
	const QString mPort;

	/// Hardware backend used to create the RGA, DMA heap and preview buffers.
	trikHal::HardwareAbstractionInterface &mHal;
	/// Display sink; may be null.
	std::shared_ptr<VideoPreviewSink> mSink;

	/// RGA scaler/color converter.
	std::unique_ptr<trikHal::RgaInterface> mRga;
	/// DMA heap for the staging buffer and camera-independent allocations.
	std::unique_ptr<trikHal::DmaHeapInterface> mDmaHeap;
	/// Staging dma-buf that holds the current camera frame for the RGA.
	std::unique_ptr<trikHal::DmaBufferInterface> mScaleInput;

	/// Number of pooled preview buffers requested per acquirePreviewBuffer().
	const int mPreviewBufferCount;

	/// Opaque handle returned by VideoPreviewSink::openPreview(); valid only
	/// while mPreviewEnabled is true; -1 when no preview is registered.
	int mPreviewId = -1;
	/// True while a preview is registered with the sink.
	bool mPreviewEnabled = false;
	/// True while the camera is acquired.
	bool mCameraAcquired = false;
	/// True when the RGA and the DMA heap are usable.
	bool mAvailable = false;
	/// Guards the "RGA failure" log message so it is printed once.
	bool mRgaErrorLogged = false;
	/// Guards the "invalid frame geometry" log message so it is printed once.
	bool mGeometryWarned = false;

	/// Set to false by stop() to end the capture loop.
	std::atomic<bool> mRunning {false};
	/// Set to true by stop() to interrupt a blocking getFrame().
	std::atomic<bool> mAbort {false};
};

} // namespace trikControl
