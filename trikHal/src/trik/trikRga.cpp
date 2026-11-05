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

#include "trikRga.h"

#include <QtGlobal>
#include <QsLog.h>

#ifdef TRIK_HAS_RGA
#	include <cstddef>
#	pragma GCC diagnostic push
#	pragma GCC diagnostic ignored "-Wpedantic"
#	include <im2d.h>
#	include <rga.h>
#	pragma GCC diagnostic pop
#endif

using namespace trikHal;
using namespace trikHal::trik;

namespace {

#ifdef TRIK_HAS_RGA
bool fail(QString *error, const QString &message)
{
	if (error) {
		*error = message;
	}
	return false;
}

int rgaFormat(RgaFormat format)
{
	switch (format) {
	case RgaFormat::Yuyv:
		return RK_FORMAT_YUYV_422;
	case RgaFormat::Nv16:
		return RK_FORMAT_YCbCr_422_SP;
	case RgaFormat::Rgb888:
		return RK_FORMAT_RGB_888;
	case RgaFormat::Bgra8888:
		return RK_FORMAT_BGRA_8888;
	case RgaFormat::Unknown:
		break;
	}

	return 0;
}

bool checkImages(const RgaImage &src, const RgaImage &dst, QString *error)
{
	if (src.fd < 0 || dst.fd < 0 || src.width <= 0 || src.height <= 0 || dst.width <= 0 || dst.height <= 0) {
		return fail(error, QStringLiteral("RGA: invalid image parameters"));
	}
	return true;
}

rga_buffer_t wrapBuffer(const RgaImage &image, int format)
{
	const auto stride = image.stride > 0 ? image.stride : image.width;
	return wrapbuffer_fd_t(image.fd, image.width, image.height, stride, image.height, format);
}

bool report(const char *operation, IM_STATUS status, QString *error)
{
	if (status == IM_STATUS_SUCCESS || status == IM_STATUS_NOERROR) {
		return true;
	}

	const auto *message = imStrError_t(status);
	if (error) {
		*error = QString::fromLatin1(message);
	}
	QLOG_WARN() << "TrikRga:" << operation << "failed:" << message;
	return false;
}
#else
bool noRga(QString *error)
{
	if (error) {
		*error = QStringLiteral("RGA support is not built");
	}
	return false;
}
#endif

} // namespace

bool TrikRga::isAvailable() const
{
#ifdef TRIK_HAS_RGA
	return true;
#else
	return false;
#endif
}

bool TrikRga::convert(const RgaImage &src, const RgaImage &dst, QString *error)
{
#ifdef TRIK_HAS_RGA
	if (!checkImages(src, dst, error)) {
		return false;
	}

	const auto srcFormat = rgaFormat(src.format);
	const auto dstFormat = rgaFormat(dst.format);
	if (srcFormat == 0 || dstFormat == 0) {
		return fail(error, QStringLiteral("RGA: unsupported image format"));
	}

	const auto srcBuffer = wrapBuffer(src, srcFormat);
	const auto dstBuffer = wrapBuffer(dst, dstFormat);
	return report("conversion",
		imcvtcolor_t(srcBuffer, dstBuffer, srcFormat, dstFormat, IM_YUV_TO_RGB_BT601_LIMIT, 1), error);
#else
	Q_UNUSED(src);
	Q_UNUSED(dst);
	return noRga(error);
#endif
}

bool TrikRga::resize(const RgaImage &src, const RgaImage &dst, QString *error)
{
#ifdef TRIK_HAS_RGA
	if (!checkImages(src, dst, error)) {
		return false;
	}

	const auto srcFormat = rgaFormat(src.format);
	const auto dstFormat = rgaFormat(dst.format);
	if (srcFormat == 0 || dstFormat == 0 || srcFormat != dstFormat) {
		return fail(error, QStringLiteral("RGA: resize requires a supported, identical format"));
	}

	const auto srcBuffer = wrapBuffer(src, srcFormat);
	const auto dstBuffer = wrapBuffer(dst, dstFormat);

	const auto status = (src.width == dst.width && src.height == dst.height)
	                            ? imcopy_t(srcBuffer, dstBuffer, 1)
	                            : imresize_t(srcBuffer, dstBuffer, static_cast<double>(dst.width) / src.width,
					    static_cast<double>(dst.height) / src.height, IM_INTERP_DEFAULT, 1);
	return report("resize", status, error);
#else
	Q_UNUSED(src);
	Q_UNUSED(dst);
	return noRga(error);
#endif
}

bool TrikRga::process(const RgaImage &src, const RgaImage &dst, QString *error)
{
#ifdef TRIK_HAS_RGA
	if (!checkImages(src, dst, error)) {
		return false;
	}

	const auto srcFormat = rgaFormat(src.format);
	const auto dstFormat = rgaFormat(dst.format);
	if (srcFormat == 0 || dstFormat == 0) {
		return fail(error, QStringLiteral("RGA: unsupported image format"));
	}

	const auto srcBuffer = wrapBuffer(src, srcFormat);
	const auto dstBuffer = wrapBuffer(dst, dstFormat);

	// Full-frame source/destination rects: RGA scales when their sizes differ
	// and converts the pixel format in the same job (BT.601 limited by default,
	// which matches the camera's YCbCr encoding).
	const auto srcRect = im_rect {0, 0, src.width, src.height};
	const auto dstRect = im_rect {0, 0, dst.width, dst.height};
	const auto patRect = im_rect {0, 0, 0, 0};
	const auto pat = rga_buffer_t {};

	const auto status = improcess(srcBuffer, dstBuffer, pat, srcRect, dstRect, patRect, IM_SYNC);
	return report("process", status, error);
#else
	Q_UNUSED(src);
	Q_UNUSED(dst);
	return noRga(error);
#endif
}
