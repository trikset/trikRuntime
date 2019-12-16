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

#include "utilites.h"

#include "QsLog.h"

QImage* Utilites::show(const QVector<int32_t> &array, int width, int height, const QString &format){
	auto fmt = QImage::Format_RGB32;
	auto *rawData = static_cast<const uchar *>(static_cast<const void *>(array.data()));

	// Reserve maximum possible size to avoid reallocation
	QVector<uchar> formattedData( (width + 3) * (height + 3) * 3 + 3);

	// QImage requires 32-bit aligned scan lines
	// Helper function to convert data
	auto copyAligned = [&](int perLine){
		auto scanLineSize = static_cast<int>((static_cast<unsigned>(perLine + 3)) & 0xFFFFFFFC);
		formattedData.resize(scanLineSize * height);
		auto dst = formattedData.begin();
		for (auto src = array.begin(); src < array.end(); src += perLine) {
			dst = std::copy(src, src + perLine, dst);
			dst += scanLineSize - perLine;
		}
		rawData = formattedData.constData();
	};

	if (!format.compare("rgb32", Qt::CaseInsensitive)) {
		/* do nothing */
	} else if (!format.compare("rgb888", Qt::CaseInsensitive)) {
		fmt = QImage::Format_RGB888;
		copyAligned(3 * width);
	} else if (format == "grayscale8") {
		fmt = QImage::Format_Grayscale8;
		copyAligned(width);
	} else {
		QLOG_ERROR() << "Unsupported format " << format;
		return nullptr;
	}

	return new QImage(rawData, width, height, fmt);
}
