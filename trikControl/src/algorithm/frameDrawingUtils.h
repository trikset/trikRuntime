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

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <cstring>

#include "cpuAlgorithmWorker.h"

namespace trikControl {

/// Writes one 32-bit BGRA pixel.
inline void writePixel(uint8_t *pixel, uint32_t color)
{
	std::memcpy(pixel, &color, sizeof(color));
}

/// Draws a vertical BGRA line, clamping it to the frame bounds.
inline void drawVerticalLine(const BgraFrameView &frame, int x, int y0, int y1, uint32_t color)
{
	if (x < 0 || x >= frame.width) {
		return;
	}

	y0 = std::max(y0, 0);
	y1 = std::min(y1, frame.height - 1);
	for (auto y = y0; y <= y1; ++y) {
		auto *pixel = frame.data + static_cast<size_t>(y) * static_cast<size_t>(frame.stride)
		              + static_cast<size_t>(x) * bgraBytesPerPixel;
		writePixel(pixel, color);
	}
}

/// Draws a horizontal BGRA line segment from @p x0 to @p x1 (inclusive),
/// clamping it to the frame bounds.
inline void drawHorizontalLine(const BgraFrameView &frame, int y, int x0, int x1, uint32_t color)
{
	if (y < 0 || y >= frame.height) {
		return;
	}

	x0 = std::max(x0, 0);
	x1 = std::min(x1, frame.width - 1);
	if (x0 > x1) {
		return;
	}

	auto *row = frame.data + static_cast<size_t>(y) * static_cast<size_t>(frame.stride);
	if (x0 == 0 && x1 == frame.width - 1 && reinterpret_cast<uintptr_t>(row) % alignof(uint32_t) == 0) {
		std::fill_n(reinterpret_cast<uint32_t *>(row), frame.width, color);
		return;
	}

	for (auto x = x0; x <= x1; ++x) {
		writePixel(row + static_cast<size_t>(x) * bgraBytesPerPixel, color);
	}
}

/// Draws a horizontal BGRA line, clamping it to the frame bounds.
inline void drawHorizontalLine(const BgraFrameView &frame, int y, uint32_t color)
{
	drawHorizontalLine(frame, y, 0, frame.width - 1, color);
}

} // namespace trikControl
