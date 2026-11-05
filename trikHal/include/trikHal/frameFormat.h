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

namespace trikHal {

/// Pixel layout of a frame. Kept backend-agnostic: the same value is mapped to
/// the RGA, DRM/EGL and (later) NPU format spaces by the corresponding
/// consumers.
enum class PixelFormat {
	Unknown,
	Bgra8888, ///< 4 bytes, memory order B,G,R,A (DRM_FORMAT_ARGB8888).
};

/// Geometry and layout of a dma-buf-backed frame.
struct FrameFormat {
	uint32_t width = 0;
	uint32_t height = 0;
	/// Bytes per line (plane 0). For packed formats this is width * bpp.
	uint32_t stride = 0;
	PixelFormat format = PixelFormat::Unknown;

	size_t byteSize() const { return static_cast<size_t>(stride) * height; }

	bool operator==(const FrameFormat &other) const
	{
		return width == other.width && height == other.height && stride == other.stride
			&& format == other.format;
	}

	bool operator!=(const FrameFormat &other) const { return !(*this == other); }
};

} // namespace trikHal
