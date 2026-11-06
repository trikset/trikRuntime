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

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <utility>

namespace trikControl {

/// 8-bit HSV value.
struct Hsv8 {
	uint8_t h = 0;
	uint8_t s = 0;
	uint8_t v = 0;
};

namespace color_detail {

/// Reciprocal table: table[i] = floor(Scale * 256 / i), table[0] = 0.
/// Values fit in 16 bits, but 32-bit entries avoid halfword arithmetic.
template<unsigned Scale, std::size_t... I>
constexpr std::array<int, 256> makeRecipTable(std::index_sequence<I...>)
{
	return {{I == 0 ? 0 : static_cast<int>((Scale * 256u) / I)...}};
}

constexpr auto kMult255 = makeRecipTable<255>(std::make_index_sequence<256> {});

constexpr auto kMult43 = makeRecipTable<43>(std::make_index_sequence<256> {});

} // namespace color_detail

/// Converts 8-bit RGB to 8-bit HSV.
inline Hsv8 rgbToHsv(int r, int g, int b)
{
	const auto mx = std::max(std::max(r, g), b);
	const auto mn = std::min(std::min(r, g), b);
	const auto delta = mx - mn;

	// kMult255[0] is 0, so the max == 0 case needs no branch.
	const auto s = (color_detail::kMult255[mx] * delta) >> 8;

	// The branch is picked by comparing max with G/B: max==R only when
	// neither G nor B equals max; G==B==max is the cyan/gray case.
	auto hue16 = 0;
	if (mx != g && mx != b) {
		hue16 = color_detail::kMult43[delta] * (g - b);
	} else if (mx == b && mx != g) {
		hue16 = 43690 + color_detail::kMult43[delta] * (r - g);
	} else {
		hue16 = 21845 + color_detail::kMult43[delta] * (b - r);
	}

	return {static_cast<uint8_t>((hue16 & 0xFFFF) >> 8), static_cast<uint8_t>(s), static_cast<uint8_t>(mx)};
}

} // namespace trikControl
