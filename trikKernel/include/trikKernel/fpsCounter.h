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
#include <chrono>
#include <cstdint>

#ifdef TRIK_DEBUG_FPS
#	include <QsLog.h>
#endif

namespace trikKernel {

/// Debug-only per-second event counter used to spot frame drops in the camera
/// pipeline (capture / DSP STEP / JPEG FIFO). Logs a rate line every few
/// seconds.
///
/// Compiled in only when the build is configured with `CONFIG += debug_fps`
/// (which defines TRIK_DEBUG_FPS, see global.pri). In a default build it is an
/// empty no-op, so it costs nothing in production.
#ifdef TRIK_DEBUG_FPS
class FpsCounter
{
public:
	explicit FpsCounter(const QString &tag, int intervalSec = 3)
		: mTag(tag)
		, mIntervalMs(intervalSec * 1000)
	{
	}

	void tick()
	{
		++mCount;
		const auto now = std::chrono::steady_clock::now();
		const auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - mLast).count();
		if (elapsed >= mIntervalMs) {
			QLOG_ERROR() << mTag << "fps:" << (mCount * 1000.0 / elapsed) << "frames:" << mCount;
			mCount = 0;
			mLast = now;
		}
	}

private:
	QString mTag;
	int mIntervalMs;
	uint32_t mCount = 0;
	std::chrono::steady_clock::time_point mLast = std::chrono::steady_clock::now();
};
#else
class FpsCounter
{
public:
	explicit FpsCounter(const QString &, int = 3)
	{
	}
	void tick()
	{
	}
};
#endif

}
