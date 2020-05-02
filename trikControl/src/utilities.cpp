/* Copyright 2019 Andrei Khodko
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

#include "utilities.h"

#include "QsLog.h"

using namespace trikControl;

QImage  Utilities::imageFromBytes(const QVector<int32_t> &array, int width, int height, const QString &format){
	// QImage requires 32-bit aligned scan lines
	// Helper function to convert data
	uchar *formattedData = nullptr;
	auto copyAligned = [&](int perLine){
		if (perLine * height > array.size()) {
			QLOG_WARN() << "imageFromBytes: not enough data";
			return;
		}
		auto scanLineSize = static_cast<int>((static_cast<unsigned>(perLine + 3)) & 0xFFFFFFFC);
		auto dst = formattedData = new uchar[scanLineSize * height];
		for (auto src = array.begin(); src < array.end(); src += perLine) {
			dst = std::copy(src, src + perLine, dst);
			dst += scanLineSize - perLine;
		}
	};

	auto fmt = QImage::Format_Invalid;

	if (!format.compare("rgb32", Qt::CaseInsensitive)) {
		const auto imageSize = width * height;
		fmt = QImage::Format_RGB32;
		if (imageSize <= array.size()) {
			formattedData = new uchar[imageSize * 4];
			auto dstPtr = static_cast<int32_t *>(static_cast<void *>(formattedData));
			std::copy(array.begin(), array.end(), dstPtr);
		}
	} else if (!format.compare("rgb888", Qt::CaseInsensitive)) {
		fmt = QImage::Format_RGB888;
		copyAligned(3 * width);
	} else if (format == "grayscale8") {
		fmt = QImage::Format_Grayscale8;
		copyAligned(1 * width);
	} else {
		QLOG_WARN() << "imageFromBytes: Unsupported format " << format;
	}

	if (!formattedData) {
		return QImage();
	} else {
		static auto cleanUp = [](void *p) { if (p) delete [](static_cast<decltype (formattedData)>(p)); };
		return QImage(formattedData, width, height, fmt, cleanUp, formattedData);
	}
}

static inline int32_t getMedian(uint8_t &a, uint8_t &b, uint8_t &c, uint8_t &d)
{
	if (a > b)
		std::swap(a, b);
	if (c > d)
		std::swap(c, d);
	if (a > c)
		std::swap(a, c);
	if (b > d)
		std::swap(b, d);
	return (static_cast<int32_t>(b) + c) >> 1;
}

QVector<int32_t> Utilities::rescalePhoto(const QVector<uchar> &data)
{
	QVector<int32_t> result;
	result.reserve(data.size() / 3); //Repack RGB88 from 3 x uint8_t into int32_t
	constexpr auto IMAGE_WIDTH = 320;
	constexpr auto IMAGE_HEIGHT = 240;
	if (data.size() >= IMAGE_WIDTH * IMAGE_HEIGHT * 3) {
		for(int row = 0; row < IMAGE_HEIGHT; row += 2) {
			for(int col = 0; col < IMAGE_WIDTH; col += 2) {
				auto row1 = &data[(row * IMAGE_WIDTH + col) * 3];
				auto row2 = row1 + IMAGE_WIDTH * 3;
				auto r1 = row1[0];
				auto g1 = row1[1];
				auto b1 = row1[2];
				auto r2 = row1[3];
				auto g2 = row1[4];
				auto b2 = row1[5];
				auto r3 = row2[0];
				auto g3 = row2[1];
				auto b3 = row2[2];
				auto r4 = row2[3];
				auto g4 = row2[4];
				auto b4 = row2[5];

				result.push_back((getMedian(r1, r2, r3, r4) << 16)
					| (getMedian(g1, g2, g3, g4) << 8)
					| getMedian(b1, b2, b3, b4));
			}
		}
	}

	return result;
}
