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

#include "dspTypes.h"

namespace trikDsp {

/// Format parameters sent to the DSP during algorithm registration.
struct AlgoDescriptor {
	PixelFormat format = PixelFormat::Unknown;
	uint32_t lineLength = 0;
};

struct DspChannel {
	QString sourceId;
	Algorithm algorithm = Algorithm::None;
	InArgs inArgs = {};
	bool videoOut = false;
	uint32_t width = 0;
	uint32_t height = 0;
	PixelFormat format = PixelFormat::Unknown; ///< Actual pixel format of the source.
	uint32_t lineLength = 0;                    ///< Actual bytes per line (V4L2 bytesperline).
	uint32_t inputBufferBase = 0;               ///< Flat index of this source's first DSP input buffer.
};

struct VideoFrame {
	const uint8_t *data = nullptr;
	size_t size = 0;
	uint32_t width = 0;
	uint32_t height = 0;
};

}
