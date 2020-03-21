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

QImage Utilities::imageFromBytes(const QVector<int32_t> &array, int width, int height, const QString &format){
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
		fmt = QImage::Format_RGB32;
		copyAligned(4 * width);
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
