/* Copyright 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include "trikHardwareAbstraction.h"

#include "trikMspI2c.h"
#include "trikMspUsb.h"
#include "trikSystemConsole.h"
#include "trikEventFile.h"
#include "trikInputDeviceFile.h"
#include "trikOutputDeviceFile.h"
#include "trikFifo.h"
#include "QsLog.h"

#include "trikV4l2VideoDevice.h"

using namespace trikHal;
using namespace trikHal::trik;

TrikHardwareAbstraction::TrikHardwareAbstraction()
	: mI2c(new TrikMspI2c())
	, mUsb(new TrikMspUsb())
	, mSystemConsole(new TrikSystemConsole())
{
}

TrikHardwareAbstraction::~TrikHardwareAbstraction()
{
}

MspI2cInterface &TrikHardwareAbstraction::mspI2c()
{
	return *mI2c.data();
}

MspUsbInterface &TrikHardwareAbstraction::mspUsb()
{
	return *mUsb.data();
}

SystemConsoleInterface &TrikHardwareAbstraction::systemConsole()
{
	return *mSystemConsole.data();
}

EventFileInterface *TrikHardwareAbstraction::createEventFile(const QString &fileName, QThread &thread) const
{
	return new TrikEventFile(fileName, thread);
}

FifoInterface *TrikHardwareAbstraction::createFifo(const QString &fileName) const
{
	return new TrikFifo(fileName);
}

InputDeviceFileInterface *TrikHardwareAbstraction::createInputDeviceFile(const QString &fileName) const
{
	return new TrikInputDeviceFile(fileName);
}

OutputDeviceFileInterface *TrikHardwareAbstraction::createOutputDeviceFile(const QString &fileName) const
{
	return new TrikOutputDeviceFile(fileName);
}

inline int clip255(int x) { return x > 255 ? 255 : x; }

QVector<uint8_t> TrikHardwareAbstraction::captureV4l2StillImage(const QString &port, const QString &pathToPic) const
{
	Q_UNUSED(pathToPic);
	TrikV4l2VideoDevice device(port);

	QLOG_INFO() << "Start open v4l2 device" << port;

	auto shot =  device.makeShot();

	QLOG_INFO() << "End capturing v4l2 from port" << port << " with " << shot.size() << "bytes";

	QVector<uint8_t> result(shot.size()/4*2*3); // YUV422 to RGB888
	constexpr auto IMAGE_WIDTH = 320;
	const auto IMAGE_HEIGHT = shot.length() / IMAGE_WIDTH / 2;
	for (auto row = 0; row < IMAGE_HEIGHT; ++row) {
		for (auto col = 0; col < IMAGE_WIDTH; col += 4) {
			auto startIndex = row * IMAGE_WIDTH + col;
			auto yuv422 = &shot[startIndex];
			auto y1 = yuv422[0] - 16;
			auto u  = yuv422[1] - 128;
			auto y2 = yuv422[2] - 16;
			auto v  = yuv422[3] - 128;
			auto _298y1 = 298 * y1;
			auto _298y2 = 298 * y2;
			auto _409v  = 409 * v;
			auto _100u  = -100 * u;
			auto _516u  = 516 * u;
			auto _208v  = -208 * v;
			auto r1 = clip255 ((_298y1 + _409v + 128)>>8);
			auto g1 = clip255 ((_298y1 + _100u + _208v + 128) >> 8);
			auto b1 = clip255 ((_298y1 + _516u + 128)>> 8);
			auto r2 = clip255 ((_298y2 + _409v + 128)>>8);
			auto g2 = clip255 ((_298y2 + _100u + _208v + 128) >> 8);
			auto b2 = clip255 ((_298y2 + _516u + 128)>> 8);
			auto rgb = &result[startIndex/4*2*3];
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


