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

#include <QString>
#include <QVector>
#include <QtGlobal>

#include "trikHal/trikHalDeclSpec.h"

namespace trikHal {

/// Element data type of a model tensor, mirroring the RKNN tensor types.
enum class RknnDataType { Uint8, Int8, Float16, Float32 };

/// Memory layout of a tensor: NCHW (channel-first) or NHWC (channel-last).
enum class RknnLayout { Nchw, Nhwc };

/// Minimal description of one model tensor, as reported by the RKNN runtime.
struct RknnTensorInfo {
	/// Tensor shape, e.g. [1, 224, 224, 3] for a NHWC image input.
	QVector<uint32_t> dims;
	/// Memory layout; selects how dims map to width/height/channels.
	RknnLayout layout = RknnLayout::Nhwc;
	/// Native element type of the model tensor.
	RknnDataType type = RknnDataType::Uint8;
	/// Tightly packed size in bytes.
	std::size_t size = 0;
	/// Real buffer size including row alignment/padding, for DMA / zero-copy.
	std::size_t sizeWithStride = 0;
	/// Row stride in elements (width rounded up to the hardware alignment).
	uint32_t wStride = 0;
};

/// Borrowed view of one output tensor.
///
/// @c data points directly into the session's NPU output buffer, so it is valid
/// only until the next run()/load()/reset() call. The caller must consume the
/// data (for example run detection post-processing) before running again.
struct RknnOutputView {
	int index = 0;
	RknnDataType type = RknnDataType::Float32;
	const void *data = nullptr;
	std::size_t size = 0; ///< Size in bytes.
	std::size_t count = 0; ///< Number of elements.
};

/// One RKNN inference session owning a loaded .rknn model.
///
/// The session is task-agnostic: it exposes the model input/output tensor
/// descriptions and a single synchronous run() over a uint8 RGB NHWC buffer.
/// Task-specific post-processing (argmax for classification, box decoding and
/// NMS for detection, ...) is done by the caller.
class TRIKHAL_EXPORT RknnSessionInterface
{
public:
	virtual ~RknnSessionInterface() = default;

	/// Loads a .rknn model. Returns false and fills @p error on failure.
	virtual bool load(const QString &modelPath, QString *error) = 0;

	virtual bool isLoaded() const = 0;

	/// Description of the single image input (width/height come from the model).
	virtual const RknnTensorInfo &input() const = 0;

	/// Descriptions of the model outputs (one or more tensors).
	virtual const QVector<RknnTensorInfo> &outputs() const = 0;

	/// Runs the loaded model using the NPU input buffer owned by the session.
	///
	/// The caller (for example the RGA scaler) must first write one uint8 RGB
	/// NHWC image into the buffer returned by inputBufferFd()/inputBufferData(),
	/// using the geometry from input(). This call is zero-copy on the input
	/// side and reads outputs directly from preallocated NPU buffers.
	///
	/// On success @p out receives one borrowed view per output tensor (float32).
	/// The views point into the session output buffers and stay valid only
	/// until the next run()/load()/reset(). Returns false and fills @p error on
	/// failure.
	virtual bool run(QVector<RknnOutputView> *out, QString *error) = 0;

	/// dma-buf descriptor of the NPU input buffer (uint8 RGB NHWC).
	///
	/// Pass it to the RGA (or another producer) as the destination, then call
	/// run(). The buffer is sized for input().sizeWithStride. Returns -1 when no
	/// model is loaded.
	virtual int inputBufferFd() const = 0;

	/// CPU mapping of the NPU input buffer, for tests and debug fills.
	///
	/// After a CPU write, run() syncs the buffer to the device. Returns nullptr
	/// when no model is loaded.
	virtual uint8_t *inputBufferData() const = 0;

	/// Time of the last run() in microseconds, 0 if nothing has run yet.
	virtual qint64 lastRunTimeUs() const = 0;
};

} // namespace trikHal
