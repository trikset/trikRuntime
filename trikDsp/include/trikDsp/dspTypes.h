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

#include <QtCore/QString>
#include <QMetaType>
#include <cstdint>
#include <trikKernel/videoUtils.h>

namespace trikDsp {

/// DSP CV algorithm identifiers.  Mirrors the C674x firmware capabilities.
enum class Algorithm { None = -1, Motion, EdgeLine, Line, Object, Mxn, Jpeg };

using trikKernel::PixelFormat;
using trikKernel::pixelFormatFromString;
using trikKernel::toV4l2Fourcc;
using trikKernel::fromV4l2Fourcc;

/// Detected target coordinates returned by the DSP.
struct Location {
	int16_t x = 0;
	int16_t y = 0;
	uint16_t size = 0;
};

struct HsvRange {
	uint8_t from = 0;
	uint8_t to = 0;
};

/// HSV detection window - all three channels (H, S, V).
struct DetectParams {
	HsvRange hue;
	HsvRange saturation;
	HsvRange value;
};

/// Arguments sent to the DSP with each frame.
struct InArgs {
	DetectParams params;
	bool autoDetect = false; ///< set by detect(), cleared on first result
	uint8_t m = 0; ///< MxN grid columns (MxnSensor only)
	uint8_t n = 0; ///< MxN grid rows    (MxnSensor only)
	uint8_t jpegQuality = 40; ///< JPEG quality 1..100 (Jpeg only)
	bool ifBlackAndWhite = false; ///< grayscale JPEG (Jpeg only)
};

/// Result returned by the DSP for one processed frame.
///
/// For Jpeg, @p jpegData is a raw pointer into the DSP output buffer that is
/// only valid until the next processFrame() call overwrites it, so the JPEG
/// consumer must run synchronously on the DSP thread right after processFrame()
/// (the pipeline delivers Jpeg results via a DirectConnection). Line/Object/Mxn
/// results ignore jpegData entirely.
struct OutArgs {
	Location location; ///< target position (Line / Object)
	DetectParams detected; ///< auto-detected HSV (non-zero when autoDetect was set)
	uint32_t colors[9] = {}; ///< MxN colour grid (MxnSensor only)
	bool autoDetect = false; ///< true if this result was produced for a frame that
	///< carried auto_detect_hsv=true (detect() one-shot)
	uint32_t jpegSize = 0; ///< encoded JPEG size in bytes (Jpeg only)
	const uint8_t *jpegData = nullptr; ///< encoded JPEG bytes (Jpeg only), valid until the
	///< next step() overwrites the shared output buffer
};

} // namespace trikDsp

Q_DECLARE_METATYPE(trikDsp::InArgs) // NOLINT(misc-no-recursion)
Q_DECLARE_METATYPE(trikDsp::OutArgs) // NOLINT(misc-no-recursion)
Q_DECLARE_METATYPE(trikDsp::Algorithm) // NOLINT(misc-no-recursion)
Q_DECLARE_METATYPE(uint32_t)
