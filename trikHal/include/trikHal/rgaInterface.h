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

#include <QString>

#include "trikHal/trikHalDeclSpec.h"

namespace trikHal {

/// Image format understood by the RGA wrapper.
enum class RgaFormat { Unknown, Yuyv, Nv16, Rgb888, Bgra8888 };

/// A dma-buf image description.
struct RgaImage {
	int fd = -1;
	int width = 0;
	int height = 0;
	RgaFormat format = RgaFormat::Unknown;
	/// Line stride in pixels (RGA's wstride); 0 means "tightly packed = width".
	/// Use it when the camera pads bytes-per-line beyond the active width.
	int stride = 0;
};

/// Hardware 2D accelerator.
///
/// All operations are synchronous. The implementation works on dma-buf file
/// descriptors, so the source and destination must be allocated from a
/// DMA-capable heap (see DmaHeapInterface).
class TRIKHAL_EXPORT RgaInterface
{
public:
	virtual ~RgaInterface() = default;

	/// True when the RGA backend is compiled in and usable.
	virtual bool isAvailable() const = 0;

	/// Convert @p src into @p dst (YUV->RGB today). Returns false when the RGA
	/// is unavailable or the driver rejects the operation; @p error receives a
	/// human-readable reason.
	virtual bool convert(const RgaImage &src, const RgaImage &dst, QString *error) = 0;

	/// Scale @p src into @p dst (same pixel format). Returns false on any
	/// failure, in which case the caller falls back to a CPU box filter.
	virtual bool resize(const RgaImage &src, const RgaImage &dst, QString *error) = 0;

	/// Compound operation: scale AND convert @p src into @p dst in one RGA job.
	/// Any supported source/destination sizes and formats are allowed.
	/// Returns false on any failure (the caller then uses the CPU YUV path).
	virtual bool process(const RgaImage &src, const RgaImage &dst, QString *error) = 0;
};

} // namespace trikHal
