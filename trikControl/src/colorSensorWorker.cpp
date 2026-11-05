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

#include "colorSensorWorker.h"

#include <algorithm>
#include <array>
#include <utility>

#include <QtCore/QMutexLocker>
#include <QtCore/QRectF>
#include <QtGui/QColor>

using namespace trikControl;

namespace {

/// hue axis split into 32 bins, saturation and value into 4.
constexpr int hueBins = 32;
constexpr int satBins = 4;
constexpr int valBins = 4;

/// Reciprocal table: table[i] = floor(Scale * 256 / i), table[0] = 0.
/// Values fit in 16 bits, but 32-bit entries avoid halfword arithmetic.
template<unsigned Scale, std::size_t... I>
constexpr std::array<int, 256> makeRecipTable(std::index_sequence<I...>)
{
	return {{I == 0 ? 0 : static_cast<int>((Scale * 256u) / I)...}};
}

constexpr auto kMult255 = makeRecipTable<255>(std::make_index_sequence<256> {});

constexpr auto kMult43 = makeRecipTable<43>(std::make_index_sequence<256> {});

/// RGB -> flat histogram bin index, fusing an RGB->HSV conversion with the
/// 32x4x4 binning. Returns (h/8) << 4 | (s/64) << 2 | (v/64) in 0..511.
/// H, S and V are in 0..255 and the hue wraps through 16 bits.
inline int rgbToBinIndex(int r, int g, int b)
{
	const auto mx = std::max(std::max(r, g), b);
	const auto mn = std::min(std::min(r, g), b);
	const auto delta = mx - mn;

	// kMult255[0] is 0, so the max == 0 case needs no branch.
	const auto s = (kMult255[mx] * delta) >> 8;

	// The branch is picked by comparing max with G/B: max==R only when
	// neither G nor B equals max; G==B==max is the cyan/gray case.
	auto hue16 = 0;
	if (mx != g && mx != b) {
		hue16 = kMult43[delta] * (g - b);
	} else if (mx == b && mx != g) {
		hue16 = 43690 + kMult43[delta] * (r - g);
	} else {
		hue16 = 21845 + kMult43[delta] * (b - r);
	}

	const auto hb = ((hue16 & 0xFFFF) >> 8) >> 3;
	const auto sb = s >> 6;
	const auto vb = mx >> 6;
	return (hb << 4) | (sb << 2) | vb;
}

/// HSV -> RGB conversion.
uint32_t hsvToRgb(int H, int S, int V)
{
	const auto h = H / 255.0;
	auto s = S / 255.0;
	auto v = V / 255.0;

	v = v < 0.2 ? 0 : v;
	s = s < 0.2 ? 0 : 1;

	const auto i = static_cast<int>(h * 6);
	const auto f = h * 6 - i;
	const auto p = v * (1 - s);
	const auto q = v * (1 - f * s);
	const auto t = v * (1 - (1 - f) * s);

	auto r = 0.0;
	auto g = 0.0;
	auto b = 0.0;
	switch (i % 6) {
	case 0:
		r = v;
		g = t;
		b = p;
		break;
	case 1:
		r = q;
		g = v;
		b = p;
		break;
	case 2:
		r = p;
		g = v;
		b = t;
		break;
	case 3:
		r = p;
		g = q;
		b = v;
		break;
	case 4:
		r = t;
		g = p;
		b = v;
		break;
	case 5:
		r = v;
		g = p;
		b = q;
		break;
	}

	const auto ri = static_cast<int>(r * 255);
	const auto gi = static_cast<int>(g * 255);
	const auto bi = static_cast<int>(b * 255);
	return (static_cast<uint32_t>(ri) << 16) | (static_cast<uint32_t>(gi) << 8) | static_cast<uint32_t>(bi);
}

/// Converts the winning bin to the RGB color returned to the script
/// (bin index -> 0..255 H/S/V -> HSVtoRGB).
uint32_t dominantColor(int hueBin, int satBin, int valBin)
{
	const auto hue = hueBin * 255 / (hueBins - 1);
	const auto sat = satBin * 255 / (satBins - 1);
	const auto val = valBin * 255 / (valBins - 1);
	return hsvToRgb(hue, sat, val);
}

} // namespace

ColorSensorWorker::ColorSensorWorker(QSharedPointer<CameraManager> camera, QString port, int m, int n,
	trikHal::HardwareAbstractionInterface &hal, std::shared_ptr<VideoPreviewSink> previewSink,
	int previewBufferCount, QObject *parent)
	: CpuAlgorithmWorker(std::move(camera), std::move(port), hal, std::move(previewSink), previewBufferCount,
		  parent)
	, mM(m)
	, mN(n)
{
}

QVector<int> ColorSensorWorker::cell(int mIndex, int nIndex) const
{
	QMutexLocker locker(&mGridLock);
	if (mIndex <= 0 || nIndex <= 0 || mIndex > mM || nIndex > mN) {
		return {-1, -1, -1};
	}

	const auto *c = mGrid[mIndex - 1][nIndex - 1].data();
	return {c[0], c[1], c[2]};
}

trikHal::FrameFormat ColorSensorWorker::scaledFormat(int srcWidth, int srcHeight) const
{
	auto format = trikHal::FrameFormat {};
	format.width = static_cast<uint32_t>(std::min(srcWidth, static_cast<int>(trikKernel::videoPreviewWidth)));
	format.height = static_cast<uint32_t>(std::min(srcHeight, static_cast<int>(trikKernel::videoPreviewHeight)));
	format.stride = format.width * 4;
	format.format = trikHal::PixelFormat::Bgra8888;
	return format;
}

bool ColorSensorWorker::processFrame(const BgraFrameView &frame)
{
	if (!frame.data || frame.width <= 0 || frame.height <= 0 || frame.stride <= 0) {
		return false;
	}

	const auto grid = buildGridFromBgra(frame);
	QMutexLocker locker(&mGridLock);
	mGrid = grid;
	return true;
}

OverlayResult ColorSensorWorker::buildOverlay(const BgraFrameView &frame)
{
	auto grid = Grid {};
	{
		QMutexLocker locker(&mGridLock);
		grid = mGrid;
	}

	auto result = OverlayResult {};
	if (!mOverlayGridValid || grid != mLastOverlayGrid) {
		mLastOverlayGrid = grid;
		mOverlayGridValid = true;
		mLastOverlay = makeOverlay(grid, frame.width, frame.height);
		++mOverlayGeneration;
	}

	result.overlay = mLastOverlay;
	result.generation = mOverlayGeneration;
	return result;
}

ColorSensorWorker::Grid ColorSensorWorker::buildGridFromBgra(const BgraFrameView &frame) const
{
	auto grid = Grid {};

	if (mM <= 0 || mN <= 0 || mM > maxColorSensorGridSize || mN > maxColorSensorGridSize) {
		return grid;
	}

	const auto cellW = frame.width / mN;
	const auto cellH = frame.height / mM;
	if (cellW <= 0 || cellH <= 0) {
		return grid;
	}

	for (auto cellRow = 0; cellRow < mM; ++cellRow) {
		for (auto cellCol = 0; cellCol < mN; ++cellCol) {
			uint32_t bins[hueBins * satBins * valBins] = {};

			const auto rowBegin = cellRow * cellH;
			const auto colBegin = cellCol * cellW;

			for (auto row = rowBegin; row < rowBegin + cellH; ++row) {
				const auto *line = frame.data + static_cast<size_t>(row) * frame.stride;
				for (auto col = colBegin; col < colBegin + cellW; ++col) {
					// DRM_FORMAT_ARGB8888 is B,G,R,A in memory on little endian.
					const auto *pixel = line + static_cast<size_t>(col) * 4;
					++bins[rgbToBinIndex(pixel[2], pixel[1], pixel[0])];
				}
			}

			auto bestIdx = 0;
			auto bestCount = 0u;
			for (auto i = 0; i < hueBins * satBins * valBins; ++i) {
				if (bins[i] > bestCount) {
					bestCount = bins[i];
					bestIdx = i;
				}
			}

			const auto bestH = bestIdx >> 4;
			const auto bestS = (bestIdx >> 2) & 0x3;
			const auto bestV = bestIdx & 0x3;
			const auto color = dominantColor(bestH, bestS, bestV);
			grid[cellRow][cellCol][0] = static_cast<int>((color >> 16) & 0xFF);
			grid[cellRow][cellCol][1] = static_cast<int>((color >> 8) & 0xFF);
			grid[cellRow][cellCol][2] = static_cast<int>(color & 0xFF);
		}
	}

	return grid;
}

PreviewOverlay ColorSensorWorker::makeOverlay(const Grid &grid, int width, int height) const
{
	auto overlay = PreviewOverlay {};

	const auto cellW = width / mN;
	const auto cellH = height / mM;
	if (cellW <= 0 || cellH <= 0) {
		return overlay;
	}

	for (auto row = 0; row < mM; ++row) {
		for (auto col = 0; col < mN; ++col) {
			const auto *c = grid[row][col].data();
			auto item = PreviewOverlayPath {};
			item.path.addRect(QRectF(col * cellW, row * cellH, 20, 20));
			item.brush = QColor(c[0], c[1], c[2]);
			overlay.append(item);
		}
	}

	return overlay;
}
