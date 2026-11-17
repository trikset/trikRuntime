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

#include <cstddef>
#include <memory>

#include <QtCore/QSize>
#include <QtCore/QVector>

#include <trikHal/rknnSessionInterface.h>

#include "videoAlgorithmWorker.h"

namespace trikControl {

/// Common NPU video-algorithm worker, built on VideoAlgorithmWorker.
///
/// The shared base owns the camera, the capture loop, RGA staging and preview
/// publishing. This worker adds an RKNN inference session and the following
/// per-frame pipeline:
///
/// 1. handleFrame() reads the model input geometry from the session (input());
/// 2. the RGA scales the staged camera frame into RGB888 directly into the
///    session input dma-buf (zero-copy, no CPU copy);
/// 3. when the preview is enabled, a second RGA pass renders the same source
///    frame into a pooled BGRA preview buffer, because the NPU input (RGB888,
///    model size) cannot be shown on screen directly;
/// 4. the preview is published with the overlay returned by buildOverlay()
///    (empty by default, so it is video-only unless a derived worker adds
///    diagnostics such as top-k labels);
/// 5. inference runs and processOutputs() receives borrowed output views.
///
/// The target resolution is never hardcoded: it comes from the loaded model,
/// which is exactly what makes the worker reusable for MobileNet, YOLO, etc.
///
/// @section threading Threading
///
/// loadModel() must be called from the control thread before start().
/// processOutputs() runs on the worker thread and must consume the views before
/// returning; they are valid only until the next run().
class NpuAlgorithmWorker : public VideoAlgorithmWorker
{
public:
	NpuAlgorithmWorker(QSharedPointer<CameraManager> camera, QString port,
		trikHal::HardwareAbstractionInterface &hal, std::shared_ptr<VideoPreviewSink> previewSink,
		int previewBufferCount = 4, QObject *parent = nullptr);

	/// Loads the .rknn model. Must be called before start().
	///
	/// The model is queried immediately: its input geometry is cached and used to
	/// program the RGA on every frame. Returns false and fills @p error on
	/// failure, in which case the worker stays not-ready.
	bool loadModel(const QString &modelPath, QString *error = nullptr);

	/// Model input geometry (dimensions, layout, stride) as reported by the
	/// loaded model. Only meaningful after a successful loadModel().
	const trikHal::RknnTensorInfo &modelInput() const;

protected:
	/// VideoAlgorithmWorker hook: runs the RGA->NPU pipeline and an optional
	/// display pass, then calls processOutputs().
	void handleFrame(Frame &frame) override;

	/// Processes the NPU output views after a successful inference.
	///
	/// Views point into the session output buffers (float32) and are valid only
	/// for the duration of this call. Classification/detection post-processing
	/// (argmax, DFL decode, NMS, ...) belongs here.
	virtual void processOutputs(const QVector<trikHal::RknnOutputView> &outputs) = 0;

	/// Overlay drawn on top of the preview, in @p previewSize pixel coordinates.
	///
	/// Called on the worker thread when the preview is on (never when it is
	/// off). The default returns an empty overlay (video-only). Derived workers
	/// use it for on-screen diagnostics such as top-k labels. Because the preview
	/// pass runs before inference, the overlay normally reflects the previous
	/// frame's result.
	virtual OverlayResult buildOverlay(const QSize &previewSize);

	/// The inference session, for example for lastRunTimeUs().
	trikHal::RknnSessionInterface &session();

	/// Camera port this worker is bound to.
	const QString &port() const;

	/// Ready only when the base resources, the session and a loaded model are
	/// all present.
	bool isReadyToStart() const override;

private:
	/// RKNN session created from the HAL; owns the model and its dma-bufs.
	std::unique_ptr<trikHal::RknnSessionInterface> mSession;

	/// Cached model input description used to program the RGA.
	trikHal::RknnTensorInfo mInputInfo;

	/// True after a successful loadModel().
	bool mLoaded = false;
};

} // namespace trikControl
