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

#include "videoDeviceFileBase.h"

namespace trikHal {
namespace trik {

/// Concrete V4L2 capture device for the target hardware. Adds format-emulation
/// reporting on top of the shared VideoDeviceFileBase logic.
class TrikVideoDevice : public trikHal::VideoDeviceFileBase
{
	Q_OBJECT
	Q_DISABLE_COPY(TrikVideoDevice)

public:
	/// Forwards capture parameters to VideoDeviceFileBase.
	TrikVideoDevice(const QString &devicePath, uint32_t width, uint32_t height, uint32_t fourcc,
		uint32_t bufferCount = trikKernel::dspInputBuffersPerRegion, bool isWebcam = false);

protected:
	/// Negotiates the format and reports whether the driver emulated it.
	bool setFormat() override;
};

} // namespace trik
} // namespace trikHal
