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

#include "colorAlgorithmWorker.h"

#include <algorithm>
#include <array>
#include <cstring>
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

/// RGB -> flat histogram bin index, fusing the DSP's RGB->HSV conversion
/// (cv_algorithms.hpp: convertRgb888ToHsv) with the 32x4x4 binning. Returns
/// (h/8) << 4 | (s/64) << 2 | (v/64) in 0..511. H, S and V are in 0..255 and
/// the hue wraps through 16 bits like the DSP's packed halfword.
inline int rgbToBinIndex(int r, int g, int b)
{
	const int mx = std::max(std::max(r, g), b);
	const int mn = std::min(std::min(r, g), b);
	const int delta = mx - mn;

	// kMult255[0] is 0, so the max == 0 case needs no branch.
	const int s = (kMult255[mx] * delta) >> 8;

	// The DSP picks the branch by comparing max with G/B: max==R only when
	// neither G nor B equals max; G==B==max is the cyan/gray case.
	int hue16 = 0;
	if (mx != g && mx != b) {
		hue16 = kMult43[delta] * (g - b);
	} else if (mx == b && mx != g) {
		hue16 = 43690 + kMult43[delta] * (r - g);
	} else {
		hue16 = 21845 + kMult43[delta] * (b - r);
	}

	const int hb = ((hue16 & 0xFFFF) >> 8) >> 3;
	const int sb = s >> 6;
	const int vb = mx >> 6;
	return (hb << 4) | (sb << 2) | vb;
}

/// HSV -> RGB exactly like the DSP (mxn_sensor.hpp: HSVtoRGB).
uint32_t hsvToRgb(int H, int S, int V)
{
	const double h = H / 255.0f;
	double s = S / 255.0f;
	double v = V / 255.0f;

	v = v < 0.2 ? 0 : v;
	s = s < 0.2 ? 0 : 1;

	const int i = static_cast<int>(h * 6);
	const double f = h * 6 - i;
	const double p = v * (1 - s);
	const double q = v * (1 - f * s);
	const double t = v * (1 - (1 - f) * s);

	double r = 0;
	double g = 0;
	double b = 0;
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

	const int ri = static_cast<int>(r * 255);
	const int gi = static_cast<int>(g * 255);
	const int bi = static_cast<int>(b * 255);
	return (static_cast<uint32_t>(ri) << 16) | (static_cast<uint32_t>(gi) << 8) | static_cast<uint32_t>(bi);
}

/// Converts the winning bin to the RGB color returned to the script, exactly
/// like the DSP (bin index -> 0..255 H/S/V -> HSVtoRGB).
uint32_t dominantColor(int hueBin, int satBin, int valBin)
{
	const int hue = hueBin * 255 / (hueBins - 1);
	const int sat = satBin * 255 / (satBins - 1);
	const int val = valBin * 255 / (valBins - 1);
	return hsvToRgb(hue, sat, val);
}

} // namespace

ColorAlgorithmWorker::ColorAlgorithmWorker(QSharedPointer<CameraManager> camera, int portId, QString port,
	int m, int n, trikHal::HardwareAbstractionInterface &hal, VideoPreviewSink *previewSink,
	int previewBufferCount, QObject *parent)
	: CpuAlgorithmWorker(std::move(camera), portId, std::move(port), hal, previewSink,
		  previewBufferCount, parent)
	, mM(m)
	, mN(n)
{
}

QVector<int> ColorAlgorithmWorker::cell(int mIndex, int nIndex) const
{
	QMutexLocker locker(&mGridLock);
	if (mIndex <= 0 || nIndex <= 0 || mIndex > mM || nIndex > mN) {
		return {-1, -1, -1};
	}

	const int *c = mGrid.colors[mIndex - 1][nIndex - 1];
	return {c[0], c[1], c[2]};
}

trikHal::FrameFormat ColorAlgorithmWorker::scaledFormat(int srcWidth, int srcHeight) const
{
	trikHal::FrameFormat format;
	format.width = static_cast<uint32_t>(std::min(srcWidth, static_cast<int>(trikKernel::videoPreviewWidth)));
	format.height = static_cast<uint32_t>(std::min(srcHeight, static_cast<int>(trikKernel::videoPreviewHeight)));
	format.stride = format.width * 4;
	format.format = trikHal::PixelFormat::Bgra8888;
	return format;
}

bool ColorAlgorithmWorker::processFrame(const BgraFrameView &frame)
{
	if (!frame.data || frame.width <= 0 || frame.height <= 0 || frame.stride <= 0) {
		return false;
	}

	const Grid grid = buildGridFromBgra(frame);
	QMutexLocker locker(&mGridLock);
	mGrid = grid;
	return true;
}

OverlayResult ColorAlgorithmWorker::buildOverlay(const BgraFrameView &frame)
{
	Grid grid;
	{
		QMutexLocker locker(&mGridLock);
		grid = mGrid;
	}

	OverlayResult result;
	if (!mOverlayGridValid || std::memcmp(&grid, &mLastOverlayGrid, sizeof(Grid)) != 0) {
		mLastOverlayGrid = grid;
		mOverlayGridValid = true;
		mLastOverlay = makeOverlay(grid, frame.width, frame.height);
		++mOverlayGeneration;
	}

	result.overlay = mLastOverlay;
	result.generation = mOverlayGeneration;
	return result;
}

ColorAlgorithmWorker::Grid ColorAlgorithmWorker::buildGridFromBgra(const BgraFrameView &frame) const
{
	Grid grid;

	if (mM <= 0 || mN <= 0 || mM > 3 || mN > 3) {
		return grid;
	}

	const int cellW = frame.width / mN;
	const int cellH = frame.height / mM;
	if (cellW <= 0 || cellH <= 0) {
		return grid;
	}

	for (int cellRow = 0; cellRow < mM; ++cellRow) {
		for (int cellCol = 0; cellCol < mN; ++cellCol) {
			uint32_t bins[hueBins * satBins * valBins] = {};

			const int rowBegin = cellRow * cellH;
			const int colBegin = cellCol * cellW;

			for (int row = rowBegin; row < rowBegin + cellH; ++row) {
				const uint8_t *line = frame.data + static_cast<size_t>(row) * frame.stride;
				for (int col = colBegin; col < colBegin + cellW; ++col) {
					// DRM_FORMAT_ARGB8888 is B,G,R,A in memory on little endian.
					const uint8_t *pixel = line + static_cast<size_t>(col) * 4;
					++bins[rgbToBinIndex(pixel[2], pixel[1], pixel[0])];
				}
			}

			int bestIdx = 0;
			uint32_t bestCount = 0;
			for (int i = 0; i < hueBins * satBins * valBins; ++i) {
				if (bins[i] > bestCount) {
					bestCount = bins[i];
					bestIdx = i;
				}
			}

			const int bestH = bestIdx >> 4;
			const int bestS = (bestIdx >> 2) & 0x3;
			const int bestV = bestIdx & 0x3;
			const uint32_t color = dominantColor(bestH, bestS, bestV);
			grid.colors[cellRow][cellCol][0] = static_cast<int>((color >> 16) & 0xFF);
			grid.colors[cellRow][cellCol][1] = static_cast<int>((color >> 8) & 0xFF);
			grid.colors[cellRow][cellCol][2] = static_cast<int>(color & 0xFF);
		}
	}

	return grid;
}

PreviewOverlay ColorAlgorithmWorker::makeOverlay(const Grid &grid, int width, int height) const
{
	PreviewOverlay overlay;

	const int cellW = width / mN;
	const int cellH = height / mM;
	if (cellW <= 0 || cellH <= 0) {
		return overlay;
	}

	for (int row = 0; row < mM; ++row) {
		for (int col = 0; col < mN; ++col) {
			const int *c = grid.colors[row][col];
			PreviewOverlayPath item;
			item.path.addRect(QRectF(col * cellW, row * cellH, 20, 20));
			item.brush = QColor(c[0], c[1], c[2]);
			overlay.append(item);
		}
	}

	return overlay;
}
