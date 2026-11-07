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
	/// Maximum number of targets kept in Result::targets.
	static constexpr int maxTargets = 8;

	/// Inclusive range of one HSV component (0..255).
	struct HsvRange {
		uint8_t from = 0; ///< Lower bound, inclusive.
		uint8_t to = 0; ///< Upper bound, inclusive.
	};

	/// HSV detection window. Hue may wrap through 0/255; S and V do not.
	///
	/// The default is an empty window (0..0 on every channel), so an unset
	/// detector matches nothing until setRanges() or detect() provides a real
	/// window. This mirrors the "no color memorized yet" state.
	struct Ranges {
		HsvRange hue; ///< Hue window; empty until set.
		HsvRange saturation; ///< Saturation window; empty until set.
		HsvRange value; ///< Value window; empty until set.
		bool hueWraps = false; ///< True when hue wraps through 0/255.
	};

	/// Read-only view of a BGRA frame.
	struct FrameView {
		const uint8_t *data = nullptr; ///< First pixel byte.
		int width = 0; ///< Frame width in pixels.
		int height = 0; ///< Frame height in pixels.
		int stride = 0; ///< Bytes per row.
	};

	/// One detected object, ordered from largest to smallest in Result.
	struct Target {
		uint16_t label = 0; ///< Connected component label.
		int x = 0; ///< Center X in frame pixels.
		int y = 0; ///< Center Y in frame pixels.
		int normalizedX = 0; ///< Center X normalized to [-100, 100].
		int normalizedY = 0; ///< Center Y normalized to [-100, 100].
		int normalizedSize = 0; ///< Size normalized to the frame extents.
		uint16_t metapixelCount = 0; ///< Number of metapixels in the cluster.
	};

	/// Detection result for one frame.
	struct Result {
		int x = 0; ///< Normalized X of the largest target.
		int y = 0; ///< Normalized Y of the largest target.
		int size = 0; ///< Normalized size of the largest target.
		std::array<Target, maxTargets> targets {}; ///< Targets, largest first.
		int targetCount = 0; ///< Number of valid entries in targets.
	};

	/// One-shot HSV range detection result.
	struct DetectResult {
		Ranges ranges; ///< Detected HSV window.
		bool valid = false; ///< True when a meaningful range was found.
	};

	/// Replaces the detection window. Invalidates the cached color lookup when
	/// the window actually changes.
	void setRanges(const Ranges &ranges);

	/// Detects targets. When @p highlight is non-null, every metapixel that
	/// belongs to a detected cluster is painted with @p highlightColor in that
	/// BGRA frame (same geometry and stride as @p frame). Painting shares the
	/// existing bitmap pass, so it does not add another frame scan.
	void process(const FrameView &frame, uint8_t *highlight = nullptr, uint32_t highlightColor = 0);

	/// One-shot HSV range detection over the central positive ROI and the
	/// outer negative ROI. Applies @p toleranceFactor to the active ranges and
	/// returns the raw detected ranges (before tolerance).
	DetectResult detect(const FrameView &frame, double toleranceFactor);

	/// Latest target-selection result.
	const Result &result() const
	{
		return mResult;
	}

private:
	/// Accumulated data of one connected component.
	struct Cluster {
		int sumX = 0; ///< Sum of metapixel X coordinates.
		int sumY = 0; ///< Sum of metapixel Y coordinates.
		uint16_t size = 0; ///< Number of metapixels.
	};

	/// Builds the metapixel bitmap and optionally highlights detected blocks.
	void buildMetapixelBitmap(const FrameView &frame, uint8_t *highlight, uint32_t highlightColor);

	/// Builds the H/S histogram over the positive and negative ROIs.
	void buildDetectHistogram(const FrameView &frame, int &modeHue, int &modeSat);

	/// Labels connected components in the metapixel bitmap.
	void clusterize();

	/// Selects the largest targets from the labelled clusters.
	void selectTargets(int frameWidth, int frameHeight);

	/// Rebuilds the RGB match lookup table for the current ranges.
	void rebuildLookup();

	/// True when both detection windows are equal.
	bool rangesEqual(const Ranges &other) const;

	Ranges mRanges; ///< Active detection window.
	std::vector<uint8_t> mLookup; ///< Packed RGB match table.
	bool mLookupValid = false; ///< True when mLookup matches mRanges.

	std::vector<uint8_t> mBitmap; ///< Matched-pixel count per metapixel.
	int mMetapixelWidth = 0; ///< Metapixels per row.
	int mMetapixelHeight = 0; ///< Number of metapixel rows.

	std::vector<uint16_t> mClusterMap; ///< Cluster label per metapixel.
	std::vector<Cluster> mClusters; ///< Cluster data indexed by label.
	std::vector<uint32_t> mStack; ///< Flood-fill stack.
	uint16_t mLabelCount = 0; ///< Number of labels, including background.

	Result mResult; ///< Latest target-selection result.

	static constexpr int detectHueBins = 32; ///< Hue histogram bins.
	static constexpr int detectSatBins = 32; ///< Saturation histogram bins.
	static constexpr int detectHueDuplicated = detectHueBins * 2; ///< Hue repeated for wrap-around.

	std::array<int, detectHueBins * detectSatBins> mDetectHistogram {}; ///< H/S histogram.
	std::array<int, (detectHueDuplicated + 1) * (detectSatBins + 1)> mDetectPrefix {}; ///< 2D prefix sums.
};

} // namespace trikControl
