/* Copyright 2024 Vladimir Kutuev
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

#include "irCameraWorker.h"

#include <array>

#include <QsLog.h>
#include <QTimer>

namespace trikControl {

IrCameraWorkerMLX90640::IrCameraWorkerMLX90640(uint8_t i2cAddr, uint16_t m, uint16_t n)
	: mI2cAddr(i2cAddr), mSensorHeight(m), mSensorWidth(n)
	, mFrameBuffer(FRAME_SIZE), mImage(IMG_SIZE), mState("Ir Camera worker")
{
	mSensorProcessBuffer.resize(mSensorHeight * mSensorWidth);
	mSensorData.resize(mSensorHeight * mSensorWidth);
}

IrCameraWorkerMLX90640::~IrCameraWorkerMLX90640()
{
}

void IrCameraWorkerMLX90640::init()
{
	if (mState.isReady()) {
		QLOG_ERROR() << "Trying to init Ir Camera that is already running, ignoring";
		return;
	}
	if (mState.status() == DeviceInterface::Status::starting) {
		// Sensor is already starting.
		QLOG_ERROR() << "Trying to start Ir Camera that is already starting, ignoring";
		return;
	}
	mState.start();

	// Library written primary in C style, so no exceptions, only error codes
	int status;

	status = MLX90640_SetRefreshRate(mI2cAddr, mRefreshRate);
	if (status != 0) {
		QString info = status == -1 ? "Got NACK." : "Written incorrect value";
		QLOG_ERROR() << "Failed to set IR camera refresh rate. " << info;
		mState.fail();
	}

	// Ensure that camera works in recommended mode
	status = MLX90640_SetChessMode(mI2cAddr);
	if (status != 0) {
		QString info = status == -1 ? "Got NACK." : "Written incorrect value";
		QLOG_ERROR() << "Failed to set IR camera mode to chess. " << info;
		mState.fail();
	}

	QVector<uint16_t> eeprom(EEPROM_SIZE);
	status = MLX90640_DumpEE(mI2cAddr, eeprom.data());
	if (status == -1) {
		QLOG_ERROR() << "Failed to dump IR camera EEPROM. Got NACK.";
		mState.fail();
	}

	status = MLX90640_ExtractParameters(eeprom.data(), &mParams);
	if (status == -7) {
		QLOG_ERROR() << "Failed extract parameters from IR camerea EEPROM.";
		mState.fail();
	}

	mState.ready();
	QLOG_INFO() << "Initialized Ir Camera worker";

	QTimer::singleShot(0, this, &IrCameraWorkerMLX90640::processFrame);
}

void IrCameraWorkerMLX90640::stop()
{
	if (mState.isReady()) {
		mState.stop();
		emit stopped();
		mState.off();
	}
}

void IrCameraWorkerMLX90640::processFrame()
{
	if (!mState.isReady()) {
		return;
	}

	int status = MLX90640_GetFrameData(mI2cAddr, mFrameBuffer.data());
	if (status < 0) {
		QString info = status == -1 ? "Got NACK." : "Frame is corrupted or can't acquire it.";
		QLOG_ERROR() << "Failed to get frame from IR. " << info;
		return;
	}

	static constexpr std::array<int32_t, 16> color_map = {
		0x00003d, 0x00009e, 0x0000ff, 0x006cef
		, 0x0097c6, 0x00b4a1, 0x00d444, 0x00ff00
		, 0x99917e, 0xb270b3, 0xfb00ff, 0xff0082
		, 0xff0000, 0xfc814e, 0xffd800, 0xffff00
	};

	for (auto chunkI = 0u, i = 0u; chunkI < mSensorHeight; ++chunkI) {
		auto hChunkDivMod = std::ldiv(IMG_HEIGHT - i, mSensorHeight - chunkI);
		auto nextChunkIStart = i + hChunkDivMod.quot;
		nextChunkIStart += (hChunkDivMod.rem) ? 1 : 0;

		for (; i < nextChunkIStart; ++i) {
			for (auto chunkJ = 0u, j = 0u; chunkJ < mSensorWidth; ++chunkJ) {
				auto wChunkDivMod = std::ldiv(IMG_WIDTH - j, mSensorWidth - chunkJ);
				auto nextChunkJStart = j + wChunkDivMod.quot;
				nextChunkJStart += (wChunkDivMod.rem) ? 1 : 0;

				for (; j < nextChunkJStart; ++j) {
					auto index = i * IMG_WIDTH + j;
					auto val = mFrameBuffer[index];
					auto param = mParams.offset[index];
					auto pixel = static_cast<int16_t>(val) - static_cast<int16_t>(param);
					mSensorProcessBuffer[chunkI * mSensorWidth + chunkJ] += pixel;
					auto &out = mImage[index];
					if (pixel < -256) {
						out = 0x000000;
						continue;
					} else if (pixel > 256) {
						out = 0xffffff;
						continue;
					}
					pixel = (pixel + 256) >> 5;
					out = color_map[pixel];
				}
			}
		}
	}
	emit newImage(mImage);

	auto heightsDivMod = std::div(IMG_HEIGHT, mSensorHeight);
	auto widthsDivMod = std::div(IMG_WIDTH, mSensorWidth);
	for (auto i = 0; i < mSensorHeight; ++i) {
		auto hCount = heightsDivMod.quot;
		hCount += (i < heightsDivMod.rem) ? 1 : 0;
		for (auto j = 0; j < mSensorWidth; ++j) {
			auto wCount = widthsDivMod.quot;
			wCount += (j < widthsDivMod.rem) ? 1 : 0;
			auto index = i * mSensorWidth + j;
			auto &in = mSensorProcessBuffer[index];
			const auto avg = in / (hCount * wCount);
			static constexpr std::array<int16_t, 6> temp = {-120, -80, -20, 20, 90, 200};
			mSensorData[index] = std::lower_bound(temp.begin(), temp.end(), avg) - temp.begin();
			// Probably faster: mSensorData[index] = std::find_if(temp.begin(), temp.end(), [avg](auto x) { return x >= avg; }) - temp.begin();
			in = 0;
		}
	}
	emit newSensorData(mSensorData);

	QTimer::singleShot(0, this, &IrCameraWorkerMLX90640::processFrame);
}

}
