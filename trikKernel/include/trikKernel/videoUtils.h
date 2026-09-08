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
#include <QtCore/QVector>
#include <QtGlobal>
#include <cstdint>

#ifdef Q_OS_LINUX
#	include <linux/videodev2.h>
#else
// V4L2 FourCC codes (v4l2_fourcc) hardcoded so this header stays portable.
static constexpr uint32_t V4L2_PIX_FMT_NV16 = 0x3631564E; // 'N', 'V', '1', '6'
static constexpr uint32_t V4L2_PIX_FMT_YUYV = 0x56595559; // 'Y', 'U', 'Y', 'V'
#endif

namespace trikKernel {

/// DSP capture geometry. Single source of truth for the ARM side; the DSP
/// firmware keeps its own copy in
/// trikDsp/trik-media-sensors/shared/include/trik/buffer.h (IMG_WIDTH/IMG_HEIGHT/
/// BUFFER_SIZE/TRIK_INPUT_*). trikDsp static_asserts these against the firmware
/// header, so a drift is caught at compile time. The firmware linker places the
/// input region (`in_buff`) at dspInputPhysAddress; VPIF DMA captures into it
/// via V4L2 USERPTR and the DSP reads each frame from base + slot offset.
constexpr uint32_t dspInputWidth = 320; ///< IMG_WIDTH
constexpr uint32_t dspInputHeight = 240; ///< IMG_HEIGHT
constexpr uint32_t dspInputFrameSize = dspInputWidth * dspInputHeight * 2; ///< BUFFER_SIZE
constexpr uint32_t dspInputRegions = 3; ///< TRIK_INPUT_REGIONS
constexpr uint32_t dspInputBuffersPerRegion = 3; ///< TRIK_INPUT_BUFFERS
constexpr uint32_t dspInputBufferTotal = dspInputRegions * dspInputBuffersPerRegion; ///< TRIK_INPUT_TOTAL
constexpr uintptr_t dspInputPhysAddress = 0xc4000100; ///< firmware linker map

enum class PixelFormat { Nv16, Yuyv, Unknown };

inline PixelFormat pixelFormatFromString(const QString &s)
{
	if (s == QStringLiteral("nv16"))
		return PixelFormat::Nv16;
	if (s == QStringLiteral("yuyv") || s == QStringLiteral("yuv422"))
		return PixelFormat::Yuyv;
	return PixelFormat::Unknown;
}

inline uint32_t toV4l2Fourcc(PixelFormat fmt)
{
	switch (fmt) {
	case PixelFormat::Nv16:
		return V4L2_PIX_FMT_NV16;
	default:
		return V4L2_PIX_FMT_YUYV;
	}
}

inline PixelFormat fromV4l2Fourcc(uint32_t fourcc)
{
	if (fourcc == V4L2_PIX_FMT_NV16)
		return PixelFormat::Nv16;
	if (fourcc == V4L2_PIX_FMT_YUYV)
		return PixelFormat::Yuyv;
	return PixelFormat::Unknown;
}

} // namespace trikKernel
