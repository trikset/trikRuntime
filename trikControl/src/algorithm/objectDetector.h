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

#include <array>
#include <cstdint>
#include <vector>

namespace trikControl {

/// Pure CPU object detector: HSV matching, metapixel bitmap, connected
/// components and target selection.
///
/// It owns all reusable working buffers and performs no heap allocations per
/// frame once the frame geometry stops growing. The detector is single-threaded
/// and has no Qt or camera dependencies, so it can be unit-tested on synthetic
/// frames and later split into tiles for parallel execution.
class ObjectDetector
{
public:
	static constexpr int metapixelSize = 4;
	static constexpr int maxTargets = 8;
	static constexpr int bytesPerPixel = 4;

	static constexpr int lookupBits = 6;
	static constexpr int lookupShift = 8 - lookupBits;
	static constexpr int lookupBinCount = 1 << lookupBits;
	static constexpr int lookupEntries = lookupBinCount * lookupBinCount * lookupBinCount;
	static constexpr int lookupBytes = (lookupEntries + 7) / 8;

	struct HsvRange {
		uint8_t from = 0;
		uint8_t to = 0;
	};

	struct Ranges {
		HsvRange hue {0, 255};
		HsvRange saturation {0, 255};
		HsvRange value {0, 255};
		bool hueWraps = false;
	};

	struct FrameView {
		const uint8_t *data = nullptr;
		int width = 0;
		int height = 0;
		int stride = 0;
	};

	struct Target {
		uint16_t label = 0;
		int x = 0;
		int y = 0;
		int normalizedX = 0;
		int normalizedY = 0;
		int normalizedSize = 0;
		uint16_t metapixelCount = 0;
	};

	struct Result {
		int x = 0;
		int y = 0;
		int size = 0;
		std::array<Target, maxTargets> targets {};
		int targetCount = 0;
	};

	struct DetectResult {
		Ranges ranges;
		bool valid = false;
	};

	void setRanges(const Ranges &ranges);
	void process(const FrameView &frame);

	/// One-shot HSV range detection over the central positive ROI and the
	/// outer negative ROI. Applies @p toleranceFactor to the active ranges and
	/// returns the raw detected ranges (before tolerance).
	DetectResult detect(const FrameView &frame, double toleranceFactor);

	const DetectResult &lastDetect() const
	{
		return mLastDetect;
	}

	const Result &result() const
	{
		return mResult;
	}

private:
	struct Cluster {
		int sumX = 0;
		int sumY = 0;
		uint16_t size = 0;
	};

	void buildMetapixelBitmap(const FrameView &frame);
	void buildDetectHistogram(const FrameView &frame, int &modeHue, int &modeSat);
	void clusterize();
	void selectTargets(int frameWidth, int frameHeight);
	void rebuildLookup();
	bool rangesEqual(const Ranges &other) const;

	Ranges mRanges;
	std::array<uint8_t, lookupBytes> mLookup {};
	bool mLookupValid = false;

	std::vector<uint8_t> mBitmap;
	int mMetapixelWidth = 0;
	int mMetapixelHeight = 0;

	std::vector<uint16_t> mClusterMap;
	std::vector<Cluster> mClusters;
	std::vector<uint32_t> mStack;
	uint16_t mLabelCount = 0;

	Result mResult;

	static constexpr int detectHueBins = 32;
	static constexpr int detectSatBins = 32;
	static constexpr int detectEmptyPenalty = 2;
	static constexpr int detectPositiveWeight = 1;
	static constexpr int detectNegativeWeight = 2;
	static constexpr int detectHueDuplicated = detectHueBins * 2;

	std::array<int, detectHueBins * detectSatBins> mDetectHistogram {};
	std::array<int, (detectHueDuplicated + 1) * (detectSatBins + 1)> mDetectPrefix {};
	DetectResult mLastDetect;
};

} // namespace trikControl
