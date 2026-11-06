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

#include "lineSensorWorker.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <limits>
#include <utility>

#include <QtCore/QMutexLocker>
#include <QtGlobal>

#include <QsLog.h>

#include "frameDrawingUtils.h"

using namespace trikControl;

namespace {

constexpr auto edgeMargin = 5; // Pixels skipped at the frame edges.
constexpr auto maxCrossBandHeight = 80; // Maximum central cross-band height.
constexpr auto minTargetPoints = quint64 {10}; // Minimum points for a valid target.

constexpr auto guideStep = 40; // Distance between vertical guide lines.
constexpr auto targetHalfLength = 99; // Half-length of the red target line.

constexpr auto detectBinCount = 256; // Number of V histogram bins.
constexpr auto emptyBinPenalty = 1; // Score penalty for an empty bin.
constexpr auto positiveWeight = 1; // Weight inside the central ROI.
constexpr auto negativeWeight = 2; // Penalty outside the outer ROI.
constexpr auto valueScale = 0.39; // V scale from 0..255 to 0..100.

constexpr auto lineHighlightB = uint8_t {255}; // Cyan blue component.
constexpr auto lineHighlightG = uint8_t {255}; // Cyan green component.
constexpr auto lineHighlightR = uint8_t {0}; // Cyan red component.

/// Magenta guide lines, BGRA byte order.
constexpr uint32_t guideColor = 0xFFFF00FF;
/// Red cross-band and target lines, BGRA byte order.
constexpr uint32_t targetColor = 0xFFFF0000;

} // namespace

LineSensorWorker::LineSensorWorker(QSharedPointer<CameraManager> camera, QString port,
	trikHal::HardwareAbstractionInterface &hal, std::shared_ptr<VideoPreviewSink> previewSink,
	int previewBufferCount, QObject *parent)
	: CpuAlgorithmWorker(std::move(camera), std::move(port), hal, std::move(previewSink), previewBufferCount,
		  parent)
{
}

void LineSensorWorker::detect()
{
	mDetectRequested.store(true, std::memory_order_release);
}

QVector<int> LineSensorWorker::detectParameters() const
{
	QMutexLocker locker(&mDetectParametersLock);
	return {mDetectParameters.hue, mDetectParameters.saturation, mDetectParameters.value,
		mDetectParameters.hueTolerance, mDetectParameters.saturationTolerance,
		mDetectParameters.valueTolerance};
}

QVector<int> LineSensorWorker::reading() const
{
	QMutexLocker locker(&mReadingLock);
	return {mReading.x, mReading.cross, mReading.mass};
}

void LineSensorWorker::detectRange(const BgraFrameView &frame)
{
	const auto hWidth = frame.width / 2;
	const auto positiveLeft = hWidth - guideStep;
	const auto positiveRight = hWidth + guideStep;
	const auto negativeLeft = positiveLeft - guideStep;
	const auto negativeRight = positiveRight + guideStep;

	std::array<int, detectBinCount> histogram {};
	int modeV = 0;
	int modeWeight = 0;

	for (int row = 0; row < frame.height; ++row) {
		const auto *line = frame.data + static_cast<size_t>(row) * static_cast<size_t>(frame.stride);
		for (int col = 0; col < frame.width; ++col) {
			int weight = 0;
			if (positiveLeft < col && positiveRight > col) {
				weight = positiveWeight;
			} else if (negativeLeft > col || negativeRight < col) {
				weight = -negativeWeight;
			} else {
				continue;
			}

			const auto *pixel = line + static_cast<size_t>(col) * bgraBytesPerPixel;
			const auto v = static_cast<int>(std::max(std::max(pixel[2], pixel[1]), pixel[0]));
			histogram[v] += weight;

			if (weight > 0 && histogram[v] > modeWeight) {
				modeWeight = histogram[v];
				modeV = v;
			}
		}
	}

	std::array<int, detectBinCount + 1> prefix {};
	for (int v = 0; v < detectBinCount; ++v) {
		const auto adjusted = histogram[v] != 0 ? histogram[v] : -emptyBinPenalty;
		prefix[v + 1] = prefix[v] + adjusted;
	}

	auto bestScore = std::numeric_limits<int>::min();
	auto bestV0 = 0;
	auto bestV1 = 0;
	auto bestDistance = std::numeric_limits<int>::max();
	auto bestWidth = std::numeric_limits<int>::max();

	for (int v0 = 0; v0 < detectBinCount; ++v0) {
		for (int v1 = v0; v1 < detectBinCount; ++v1) {
			const auto score = prefix[v1 + 1] - prefix[v0];
			const auto distance = modeV < v0 ? v0 - modeV : (modeV > v1 ? modeV - v1 : 0);
			const auto width = v1 - v0;
			const auto better = score > bestScore
			                    || (score == bestScore
						    && (distance < bestDistance
							    || (distance == bestDistance && width < bestWidth)));
			if (!better) {
				continue;
			}

			bestScore = score;
			bestV0 = v0;
			bestV1 = v1;
			bestDistance = distance;
			bestWidth = width;
		}
	}

	auto valueFrom = bestV0;
	auto valueTo = bestV1;
	if (mToleranceFactor != 1.0) {
		const auto center = (bestV0 + bestV1) / 2.0;
		const auto half = (bestV1 - bestV0) / 2.0 * mToleranceFactor;
		valueFrom = static_cast<int>(std::max(0.0, center - half));
		valueTo = static_cast<int>(std::min(255.0, center + half));
	}

	mHueRange = {0, 255};
	mSaturationRange = {0, 255};
	mValueRange = {static_cast<uint8_t>(valueFrom), static_cast<uint8_t>(valueTo)};
	mHueWraps = false;

	DetectParameters parameters;
	const auto scaledFrom = bestV0 * valueScale;
	const auto scaledTo = (bestV1 + 1) * valueScale;
	parameters.value = static_cast<int>((scaledFrom + scaledTo) / 2);
	parameters.valueTolerance = static_cast<int>((scaledTo - scaledFrom) / 2);

	QLOG_INFO() << "LineSensorWorker: detect V" << bestV0 << '-' << bestV1 << "mode" << modeV << "applied"
		    << valueFrom << '-' << valueTo;

	QMutexLocker locker(&mDetectParametersLock);
	mDetectParameters = parameters;
}

bool LineSensorWorker::matches(const Hsv8 &hsv) const
{
	const auto hueMatches = mHueWraps ? (hsv.h >= mHueRange.from || hsv.h <= mHueRange.to)
	                                  : (hsv.h >= mHueRange.from && hsv.h <= mHueRange.to);

	return hueMatches && hsv.s >= mSaturationRange.from && hsv.s <= mSaturationRange.to && hsv.v >= mValueRange.from
	       && hsv.v <= mValueRange.to;
}

// TODO: rewrite this using NEON vector instructions. For now a frame takes ~3-4 ms, so it is not a bottleneck yet.
bool LineSensorWorker::processFrame(const BgraFrameView &frame)
{
	if (!frame.data || frame.width <= 0 || frame.height <= 0 || frame.stride < frame.width * bgraBytesPerPixel) {
		return false;
	}

	if (mDetectRequested.exchange(false, std::memory_order_acq_rel)) {
		detectRange(frame);
	}

	const auto bandStart = frame.height / 2;
	const auto bandHeight = std::min(maxCrossBandHeight, frame.height - bandStart);
	const auto bandStop = bandStart + bandHeight;

	quint64 points = 0;
	qint64 sumX = 0;
	quint64 crossPoints = 0;

	for (auto row = 0; row < frame.height; ++row) {
		auto *line = frame.data + static_cast<size_t>(row) * static_cast<size_t>(frame.stride);
		const auto inBand = row >= bandStart && row < bandStop;

		for (auto col = edgeMargin; col <= frame.width - edgeMargin; ++col) {
			auto *pixel = line + static_cast<size_t>(col) * bgraBytesPerPixel;
			const auto hsv = rgbToHsv(pixel[2], pixel[1], pixel[0]);
			if (!matches(hsv)) {
				continue;
			}

			pixel[0] = lineHighlightB;
			pixel[1] = lineHighlightG;
			pixel[2] = lineHighlightR;

			++points;
			sumX += col;
			if (inBand) {
				++crossPoints;
			}
		}
	}

	const auto hWidth = frame.width / 2;
	const auto hHeight = frame.height / 2;

	drawVerticalLine(frame, hWidth - 2 * guideStep, 0, frame.height - 1, guideColor);
	drawVerticalLine(frame, hWidth - guideStep, 0, frame.height - 1, guideColor);
	drawVerticalLine(frame, hWidth + guideStep, 0, frame.height - 1, guideColor);
	drawVerticalLine(frame, hWidth + 2 * guideStep, 0, frame.height - 1, guideColor);

	drawHorizontalLine(frame, bandStart, targetColor);
	drawHorizontalLine(frame, std::min(frame.height - 1, bandStart + maxCrossBandHeight), targetColor);

	LineReading reading;
	if (points > minTargetPoints) {
		const auto targetX = static_cast<int>(sumX / static_cast<qint64>(points));
		reading.x = ((targetX - frame.width / 2) * 200) / frame.width;
		reading.cross = static_cast<int>(
			crossPoints * 100 / (static_cast<quint64>(frame.width) * static_cast<quint64>(bandHeight)));
		reading.mass = static_cast<int>(
			points * 100 / (static_cast<quint64>(frame.width) * static_cast<quint64>(frame.height)));

		drawVerticalLine(frame, targetX, std::max(0, hHeight - targetHalfLength),
			std::min(frame.height - 1, hHeight + targetHalfLength), targetColor);
	}

	QMutexLocker locker(&mReadingLock);
	mReading = reading;
	return true;
}

OverlayResult LineSensorWorker::buildOverlay(const BgraFrameView &frame)
{
	Q_UNUSED(frame);
	return {};
}
