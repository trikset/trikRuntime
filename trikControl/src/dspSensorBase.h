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

#include <QtCore/QVector>
#include <QtGlobal>

#include <algorithm>
#include <cstdint>

#include <trikDsp/dspTypes.h>

#include "deviceState.h"

namespace trikKernel {
class Configurer;
}

namespace trikControl {

/// Shared plumbing for a DSP-based video sensor: holds the device state, the
/// DSP InArgs (the algorithm parameters forwarded to the DspServer on every
/// activation) and the videoOut flag. Concrete sensors own a DspSensorHelper
/// member and only implement their protocol (detect/read/onResult).
class DspSensorHelper final
{
public:
	/// Records the sensor identity and the DSP algorithm it drives.
	DspSensorHelper(const QString &name, const trikKernel::Configurer &configurer,
	                const QString &port, trikDsp::Algorithm algo);

	trikDsp::Algorithm algorithm() const { return mAlgo; }

	DeviceState &state() { return mState; }
	const DeviceState &state() const { return mState; }
	const trikKernel::Configurer &configurer() const { return mConfigurer; }
	const QString &port() const { return mPort; }

	trikDsp::InArgs &inArgs() { return mInArgs; }
	const trikDsp::InArgs &inArgs() const { return mInArgs; }
	bool videoOut() const { return mVideoOut; }

	/// Marks the sensor as initialized and records whether the video must be
	/// shown on the display. Returns false (and stays failed) when the sensor
	/// is in a failed state.
	bool doInit(bool showOnDisplay);

	/// Clears the videoOut flag; called when the sensor is stopped.
	void doStop();

private:
	DeviceState mState;
	const trikKernel::Configurer &mConfigurer;
	const QString mPort;
	trikDsp::Algorithm mAlgo;
	trikDsp::InArgs mInArgs;
	bool mVideoOut = false;
};

/// Convert an auto-detected [from, to] HSV range into the legacy
/// getDetectParameters() representation: hue, saturation and value centers
/// followed by their tolerances (6 ints, the documented contract).
inline QVector<int> toDetectParameters(const trikDsp::DetectParams &p)
{
	return {
		(p.hue.from + p.hue.to) / 2,
		(p.saturation.from + p.saturation.to) / 2,
		(p.value.from + p.value.to) / 2,
		(p.hue.to - p.hue.from) / 2,
		(p.saturation.to - p.saturation.from) / 2,
		(p.value.to - p.value.from) / 2
	};
}

/// Widen each [from, to] HSV range around its center by @p factor (clamped to
/// [0, 255]). Restores the old toleranceFactor behaviour: the factor scales the
/// auto-detected tolerance before the params are fed back to the DSP.
inline void applyToleranceFactor(trikDsp::DetectParams &p, qreal factor)
{
	const auto widen = [factor](trikDsp::HsvRange &range) {
		const auto center = (static_cast<int>(range.from) + static_cast<int>(range.to)) / 2;
		const auto tolerance = static_cast<int>((range.to - range.from) / 2.0 * factor);
		range.from = static_cast<uint8_t>(std::max(0, center - tolerance));
		range.to = static_cast<uint8_t>(std::min(255, center + tolerance));
	};
	widen(p.hue);
	widen(p.saturation);
	widen(p.value);
}

}

