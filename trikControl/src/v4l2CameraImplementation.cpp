/* Copyright 2018 Ivan Tyulyandin and CyberTech Labs Ltd.
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

#include "v4l2CameraImplementation.h"
#include "cameraManager.h"

#include <QtCore/QVector>
#include <QtGui/QImage>
#include <QsLog.h>
#include <linux/videodev2.h>

using namespace trikControl;

namespace {

inline unsigned char clip255(int x)
{
	return x >= 255 ? 255 : (x <= 0 ? 0 : static_cast<uint8_t>(x));
}

QVector<uint8_t> yuyvToRgb(const uint8_t *shot, int height, int width)
{
	QVector<uint8_t> result(height * width * 3);
	int startIndex = 0;
	for (auto row = 0; row < height; ++row) {
		for (auto col = 0; col < width; col += 2) {
			auto y0 = shot[startIndex];
			auto cb = shot[startIndex + 1];
			auto y1 = shot[startIndex + 2];
			auto cr = shot[startIndex + 3];
			startIndex += 4;

			auto resRgb = &result[(row * width + col) * 3];
			const auto alpha = 180 * (cr - 128) / 128;
			const auto beta = 45 * (cb - 128) / 128;
			resRgb[0] = clip255(y0 + alpha);
			resRgb[1] = clip255(y0 - beta - alpha / 2);
			resRgb[2] = clip255(y0 + 5 * beta);
			resRgb[3] = clip255(y1 + alpha);
			resRgb[4] = clip255(y1 - beta - alpha / 2);
			resRgb[5] = clip255(y1 + 5 * beta);
		}
	}
	return result;
}

QVector<uint8_t> yuv422pToRgb(const uint8_t *shot, int height, int width)
{
	QVector<uint8_t> result(height * width * 3);
	if (width <= 0 || height <= 0)
		return result;

	const auto Y = shot;
	const auto UV = shot + width * height;

	for (auto row = 0; row < height; ++row) {
		for (auto col = 0; col < width; col += 2) {
			auto startIndex = row * width + col;
			int const y1 = Y[startIndex] - 16;
			int const y2 = Y[startIndex + 1] - 16;
			int const u = UV[startIndex] - 128;
			int const v = UV[startIndex + 1] - 128;
			auto _298y1 = 298 * y1;
			auto _298y2 = 298 * y2;
			auto _409v = 409 * v;
			auto _100u = -100 * u;
			auto _516u = 516 * u;
			auto _208v = -208 * v;
			auto r1 = clip255((_298y1 + _516u + 128) >> 8);
			auto g1 = clip255((_298y1 + _100u + _208v + 128) >> 8);
			auto b1 = clip255((_298y1 + _409v + 128) >> 8);
			auto r2 = clip255((_298y2 + _516u + 128) >> 8);
			auto g2 = clip255((_298y2 + _100u + _208v + 128) >> 8);
			auto b2 = clip255((_298y2 + _409v + 128) >> 8);

			auto rgb = &result[startIndex * 3];
			rgb[0] = r1;
			rgb[1] = g1;
			rgb[2] = b1;
			rgb[3] = r2;
			rgb[4] = g2;
			rgb[5] = b2;
		}
	}
	return result;
}

}

V4l2CameraImplementation::V4l2CameraImplementation(const QString &port,  // NOLINT(modernize-pass-by-value)
	const QSharedPointer<CameraManager> &cameraManager)
	: mPort(port)
	, mCameraManager(cameraManager)
{
}

QVector<uint8_t> V4l2CameraImplementation::getPhoto()
{
	// This method runs on a dedicated worker thread (CameraDevice::getPhoto).
	// Pull the newest frame with a watchdog timeout: acquire() is asynchronous,
	// so frames only start flowing once the manager thread has opened the device.
	mCameraManager->acquire(mPort);

	const uint32_t fourcc = mCameraManager->fourcc(mPort);
	const int width = static_cast<int>(mCameraManager->width(mPort));
	const int height = static_cast<int>(mCameraManager->height(mPort));

	QVector<uint8_t> rgb;
	{
		const Frame frame = mCameraManager->getFrame(mPort, 0, 5000);
		if (!frame.isValid() || !frame.data() || width <= 0 || height <= 0) {
			QLOG_ERROR() << "V4l2CameraImplementation: no frame received within 5s on" << mPort;
			mCameraManager->release(mPort);
			return {};
		}

		switch (fourcc) {
		case V4L2_PIX_FMT_NV16:
			rgb = yuv422pToRgb(frame.data(), height, width);
			break;
		case V4L2_PIX_FMT_YUYV:
			rgb = yuyvToRgb(frame.data(), height, width);
			break;
		default:
			break;
		}
		// The frame handle is dropped here: its capture buffer is returned to
		// the driver automatically once nobody references it anymore.
	}

	// Our camera hold must not outlive the frame (the device is stopped/closed
	// on the last release, which would invalidate the buffer).
	mCameraManager->release(mPort);

	if (rgb.isEmpty())
		return {};

	const QImage image(rgb.data(), width, height, QImage::Format_RGB888);
	image.save(getTempDir().filePath("photo.jpg"), "JPG");
	return rgb;
}
