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
#include <cstdint>

#include <QVector>

#include <trikHal/rknnSessionInterface.h>

// Library header is installed into the SDK sysroot. This translation unit is
// only compiled when TRIK_HAS_RKNN is defined (the real target build), so the
// header is always available here.
#include "rknn_api.h"

namespace trikHal {
namespace trik {

/// RKNN inference session backed by librknnrt.
///
/// Owns one rknn_context for a single loaded .rknn model. Typical lifetime:
/// load() once, then run() as often as needed, then the destructor destroys the
/// context. The class is not internally serialized: callers must not call
/// run() concurrently from several threads. A single instance used from one
/// worker thread (as the video pipeline does) is fine.
///
/// The session is task-agnostic. It only moves tensors in and out of the NPU;
/// classification/detection/... post-processing is done by the caller.
class TrikRknnSession : public RknnSessionInterface
{
public:
	TrikRknnSession();
	~TrikRknnSession() override;

	/// Loads @p modelPath (.rknn) and queries its input/output tensors.
	///
	/// Any previously loaded model is destroyed first, so the same session can
	/// be reused for another model. Only single-image-input models are accepted
	/// for now. Returns false and fills @p error (when non-null) on failure; in
	/// that case the session stays unloaded.
	bool load(const QString &modelPath, QString *error) override;

	/// True after a successful load() and before destruction/reset.
	bool isLoaded() const override;

	/// Description of the single image input as reported by the model.
	///
	/// The width/height come from the model itself (e.g. 224x224 or 640x640),
	/// together with the native element type, memory layout, tightly packed
	/// size, DMA buffer size (sizeWithStride) and row stride (wStride). This is
	/// what a video worker uses to configure the RGA destination and the input
	/// dma-buf. The returned reference is only valid while the model is loaded.
	const RknnTensorInfo &input() const override;

	/// Descriptions of the model outputs (one or more tensors).
	///
	/// Same lifetime rules as input(). The count and shapes depend on the model
	/// (e.g. a single [1, 1000] vector for classification, several tensors for
	/// detection).
	const QVector<RknnTensorInfo> &outputs() const override;

	/// Runs inference using the session-owned input buffer.
	///
	/// The producer (RGA or CPU, via inputBufferFd()/inputBufferData()) must have
	/// written one uint8 RGB NHWC image, sized by input(). This is zero-copy on
	/// the input side; outputs are read straight from preallocated NPU buffers.
	///
	/// On success @p out receives one borrowed view per output tensor (float32),
	/// valid until the next run()/load()/reset(). Returns false and fills
	/// @p error on failure.
	bool run(QVector<RknnOutputView> *out, QString *error) override;

	/// dma-buf descriptor of the NPU input buffer, or -1 when no model is loaded.
	int inputBufferFd() const override;

	/// CPU mapping of the NPU input buffer, or nullptr when no model is loaded.
	uint8_t *inputBufferData() const override;

	/// Inference time of the last successful run() in microseconds.
	qint64 lastRunTimeUs() const override;

private:
	/// Destroys the RKNN context (if any) and resets all cached model data.
	void reset();

	/// librknnrt context; 0 when no model is loaded.
	rknn_context mCtx = 0;
	/// True only between a successful load() and reset()/destruction.
	bool mLoaded = false;
	/// Cached input tensor description of the loaded model.
	RknnTensorInfo mInput;
	/// Cached output tensor descriptions of the loaded model.
	QVector<RknnTensorInfo> mOutputs;

	/// Input tensor attributes (forced to uint8 NHWC) bound to mInputMem.
	rknn_tensor_attr mInputAttr {};
	/// NPU input buffer written by RGA/CPU and consumed by the model.
	rknn_tensor_mem *mInputMem = nullptr;
	/// Output tensor attributes, each forced to float32.
	QVector<rknn_tensor_attr> mOutputAttrs;
	/// Preallocated NPU output buffers, read directly after each run.
	QVector<rknn_tensor_mem *> mOutputMems;

	/// Duration of the last run() in microseconds, 0 before the first run.
	qint64 mLastRunUs = 0;
};

} // namespace trik
} // namespace trikHal
