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

#include "objectDetector.h"

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <limits>

#include "colorUtils.h"

using namespace trikControl;

void ObjectDetector::setRanges(const Ranges &ranges)
{
	if (rangesEqual(ranges)) {
		return;
	}

	mRanges = ranges;
	mLookupValid = false;
}

bool ObjectDetector::rangesEqual(const Ranges &other) const
{
	return mRanges.hue.from == other.hue.from && mRanges.hue.to == other.hue.to
	       && mRanges.saturation.from == other.saturation.from && mRanges.saturation.to == other.saturation.to
	       && mRanges.value.from == other.value.from && mRanges.value.to == other.value.to
	       && mRanges.hueWraps == other.hueWraps;
}

void ObjectDetector::rebuildLookup()
{
	mLookup.fill(0);

	const auto binToValue = [](int bin) {
		constexpr auto maxBin = lookupBinCount - 1;
		return (bin * 255 + maxBin / 2) / maxBin;
	};

	for (auto redBin = 0; redBin < lookupBinCount; ++redBin) {
		const auto red = binToValue(redBin);

		for (auto greenBin = 0; greenBin < lookupBinCount; ++greenBin) {
			const auto green = binToValue(greenBin);

			for (auto blueBin = 0; blueBin < lookupBinCount; ++blueBin) {
				const auto blue = binToValue(blueBin);
				const auto hsv = rgbToHsv(red, green, blue);
				const auto hueMatches =
					mRanges.hueWraps ? (hsv.h >= mRanges.hue.from || hsv.h <= mRanges.hue.to)
							 : (hsv.h >= mRanges.hue.from && hsv.h <= mRanges.hue.to);

				if (!hueMatches || hsv.s < mRanges.saturation.from || hsv.s > mRanges.saturation.to
					|| hsv.v < mRanges.value.from || hsv.v > mRanges.value.to) {
					continue;
				}

				const auto index = (redBin << (2 * lookupBits)) | (greenBin << lookupBits) | blueBin;
				mLookup[static_cast<size_t>(index >> 3)] |= static_cast<uint8_t>(1u << (index & 7));
			}
		}
	}

	mLookupValid = true;
}

void ObjectDetector::process(const FrameView &frame)
{
	mResult = Result {};

	if (!frame.data || frame.width <= 0 || frame.height <= 0 || frame.stride < frame.width * bytesPerPixel) {
		mBitmap.clear();
		mClusterMap.clear();
		mClusters.clear();
		mLabelCount = 0;
		mMetapixelWidth = 0;
		mMetapixelHeight = 0;
		return;
	}

	if (!mLookupValid) {
		rebuildLookup();
	}

	buildMetapixelBitmap(frame);
	clusterize();
	selectTargets(frame.width, frame.height);
}

void ObjectDetector::buildMetapixelBitmap(const FrameView &frame)
{
	mMetapixelWidth = frame.width / metapixelSize;
	mMetapixelHeight = frame.height / metapixelSize;

	const auto metapixelWidth = mMetapixelWidth;
	const auto metapixelHeight = mMetapixelHeight;
	if (metapixelWidth <= 0 || metapixelHeight <= 0) {
		mBitmap.clear();
		return;
	}

	const auto size = static_cast<size_t>(metapixelWidth) * static_cast<size_t>(metapixelHeight);
	if (mBitmap.size() != size) {
		mBitmap.resize(size);
	}

	const auto *lookup = mLookup.data();

	for (auto metapixelY = 0; metapixelY < metapixelHeight; ++metapixelY) {
		const uint8_t *rowLines[metapixelSize];
		for (auto dy = 0; dy < metapixelSize; ++dy) {
			const auto row = metapixelY * metapixelSize + dy;
			rowLines[dy] = frame.data + static_cast<size_t>(row) * static_cast<size_t>(frame.stride);
		}

		for (auto metapixelX = 0; metapixelX < metapixelWidth; ++metapixelX) {
			auto count = uint8_t {0};
			const auto columnOffset = static_cast<size_t>(metapixelX * metapixelSize) * bytesPerPixel;

			for (const auto *line : rowLines) {
				const auto *pixel = line + columnOffset;

				for (auto dx = 0; dx < metapixelSize; ++dx, pixel += bytesPerPixel) {
					auto packed = uint32_t {0};
					std::memcpy(&packed, pixel, sizeof(packed));

					const auto blue = static_cast<int>(packed & 0xFF) >> lookupShift;
					const auto green = static_cast<int>((packed >> 8) & 0xFF) >> lookupShift;
					const auto red = static_cast<int>((packed >> 16) & 0xFF) >> lookupShift;
					const auto index = (red << (2 * lookupBits)) | (green << lookupBits) | blue;

					count += static_cast<uint8_t>(
						(lookup[static_cast<size_t>(index >> 3)] >> (index & 7)) & 1u);
				}
			}

			mBitmap[static_cast<size_t>(metapixelY) * static_cast<size_t>(metapixelWidth)
				+ static_cast<size_t>(metapixelX)] = count;
		}
	}
}

void ObjectDetector::clusterize()
{
	const auto width = mMetapixelWidth;
	const auto height = mMetapixelHeight;
	const auto count = static_cast<size_t>(width) * static_cast<size_t>(height);

	if (width <= 0 || height <= 0 || mBitmap.size() != count || count >= std::numeric_limits<uint16_t>::max()) {
		mClusterMap.clear();
		mClusters.clear();
		mLabelCount = 0;
		return;
	}

	const auto capacity = count + 1;
	if (mClusters.size() < capacity) {
		mClusters.resize(capacity);
	}

	if (mClusterMap.size() != count) {
		mClusterMap.resize(count);
	}
	std::fill(mClusterMap.begin(), mClusterMap.end(), uint16_t {0});

	if (mStack.capacity() < count) {
		mStack.reserve(count);
	}

	auto *clusters = mClusters.data();
	auto *clusterMap = mClusterMap.data();
	const auto *bitmap = mBitmap.data();

	auto nextLabel = uint16_t {1};

	for (auto y = 0; y < height; ++y) {
		const auto rowOffset = static_cast<size_t>(y) * static_cast<size_t>(width);

		for (auto x = 0; x < width; ++x) {
			const auto index = rowOffset + static_cast<size_t>(x);
			if (bitmap[index] <= metapixelSize / 2 || clusterMap[index] != 0) {
				continue;
			}

			const auto label = nextLabel++;
			clusters[label] = Cluster {};
			clusterMap[index] = label;

			mStack.clear();
			mStack.push_back(
				static_cast<uint32_t>((static_cast<uint32_t>(y) << 16) | static_cast<uint32_t>(x)));

			while (!mStack.empty()) {
				const auto current = mStack.back();
				mStack.pop_back();

				const auto currentX = static_cast<int>(current & 0xFFFF);
				const auto currentY = static_cast<int>(current >> 16);

				auto &cluster = clusters[label];
				cluster.sumX += currentX;
				cluster.sumY += currentY;
				++cluster.size;

				const auto tryAddNeighbor = [&](size_t neighborIndex, int neighborX, int neighborY) {
					if (bitmap[neighborIndex] > metapixelSize / 2
						&& clusterMap[neighborIndex] == 0) {
						clusterMap[neighborIndex] = label;
						mStack.push_back(
							static_cast<uint32_t>((static_cast<uint32_t>(neighborY) << 16)
									      | static_cast<uint32_t>(neighborX)));
					}
				};

				if (currentX > 0 && currentX + 1 < width && currentY > 0 && currentY + 1 < height) {
					const auto currentIndex =
						static_cast<size_t>(currentY) * static_cast<size_t>(width)
						+ static_cast<size_t>(currentX);

					tryAddNeighbor(currentIndex - static_cast<size_t>(width) - 1, currentX - 1,
						currentY - 1);
					tryAddNeighbor(currentIndex - static_cast<size_t>(width), currentX,
						currentY - 1);
					tryAddNeighbor(currentIndex - static_cast<size_t>(width) + 1, currentX + 1,
						currentY - 1);
					tryAddNeighbor(currentIndex - 1, currentX - 1, currentY);
					tryAddNeighbor(currentIndex + 1, currentX + 1, currentY);
					tryAddNeighbor(currentIndex + static_cast<size_t>(width) - 1, currentX - 1,
						currentY + 1);
					tryAddNeighbor(currentIndex + static_cast<size_t>(width), currentX,
						currentY + 1);
					tryAddNeighbor(currentIndex + static_cast<size_t>(width) + 1, currentX + 1,
						currentY + 1);
				} else {
					const auto xBegin = currentX > 0 ? currentX - 1 : 0;
					const auto xEnd = currentX + 1 < width ? currentX + 1 : width - 1;
					const auto yBegin = currentY > 0 ? currentY - 1 : 0;
					const auto yEnd = currentY + 1 < height ? currentY + 1 : height - 1;

					for (auto neighborY = yBegin; neighborY <= yEnd; ++neighborY) {
						const auto neighborRow =
							static_cast<size_t>(neighborY) * static_cast<size_t>(width);

						for (auto neighborX = xBegin; neighborX <= xEnd; ++neighborX) {
							if (neighborX == currentX && neighborY == currentY) {
								continue;
							}

							tryAddNeighbor(neighborRow + static_cast<size_t>(neighborX),
								neighborX, neighborY);
						}
					}
				}
			}
		}
	}

	mLabelCount = nextLabel;
}

void ObjectDetector::selectTargets(int frameWidth, int frameHeight)
{
	std::array<Target, maxTargets> targets {};
	auto targetCount = 0;

	if (frameWidth > 0 && frameHeight > 0 && mMetapixelWidth > 0 && mMetapixelHeight > 0 && mLabelCount > 1) {
		const auto metapixelLinearSum = mMetapixelWidth + mMetapixelHeight;
		const auto *clusters = mClusters.data();

		const auto better = [](const Target &first, const Target &second) {
			return first.metapixelCount > second.metapixelCount
			       || (first.metapixelCount == second.metapixelCount && first.label < second.label);
		};

		for (uint16_t label = 1; label < mLabelCount; ++label) {
			const auto &cluster = clusters[label];
			if (cluster.size == 0) {
				continue;
			}

			const auto metapixelCount = static_cast<int>(cluster.size);
			const auto linearSize = static_cast<int>(std::sqrt(static_cast<float>(metapixelCount)));
			const auto targetRadius =
				static_cast<int>(std::ceil(static_cast<float>(linearSize) / 3.1415927f));
			const auto normalizedSize = targetRadius * 100 * metapixelSize / metapixelLinearSum;
			if (normalizedSize <= 4) {
				continue;
			}

			const auto x = (cluster.sumX / metapixelCount) * metapixelSize;
			const auto y = (cluster.sumY / metapixelCount) * metapixelSize;

			auto target = Target {};
			target.label = label;
			target.x = x;
			target.y = y;
			target.normalizedX = ((x - frameWidth / 2) * 100 * 2) / frameWidth;
			target.normalizedY = ((y - frameHeight / 2) * 100 * 2) / frameHeight;
			target.normalizedSize = normalizedSize;
			target.metapixelCount = cluster.size;

			auto position = targetCount;
			while (position > 0 && better(target, targets[position - 1])) {
				if (position < maxTargets) {
					targets[position] = targets[position - 1];
				}
				--position;
			}

			if (position < maxTargets) {
				targets[position] = target;
				if (targetCount < maxTargets) {
					++targetCount;
				}
			}
		}
	}

	mResult.targets = targets;
	mResult.targetCount = targetCount;

	if (targetCount > 0) {
		mResult.x = targets.front().normalizedX;
		mResult.y = targets.front().normalizedY;
		mResult.size = targets.front().normalizedSize;
	}
}
