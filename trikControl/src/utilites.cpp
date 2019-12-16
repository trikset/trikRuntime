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
