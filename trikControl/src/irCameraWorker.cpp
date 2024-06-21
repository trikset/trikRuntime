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

	for (auto chunkI = 0u, i = 0u; chunkI < mSensorHeight; ++chunkI) {
		auto nextChunkIStart = i + (IMG_HEIGHT - i) / (mSensorHeight - chunkI);
		nextChunkIStart += ((IMG_HEIGHT - i) % (mSensorHeight - chunkI)) ? 1 : 0;

		for (; i < nextChunkIStart; ++i) {
			for (auto chunkJ = 0u, j = 0u; chunkJ < mSensorWidth; ++chunkJ) {
				auto nextChunkJStart = j + (IMG_WIDTH - j) / (mSensorWidth - chunkJ);
				nextChunkJStart += ((IMG_WIDTH - j) % (mSensorWidth - chunkJ)) ? 1 : 0;

				for (; j < nextChunkJStart; ++j) {
					auto val = mFrameBuffer[i * IMG_WIDTH + j];
					auto pixel = static_cast<int16_t>(val) - static_cast<int16_t>(mParams.offset[i * IMG_WIDTH + j]);
					mSensorProcessBuffer[chunkI * mSensorWidth + chunkJ] += pixel;
					if (pixel < -256) {
						mImage[i * IMG_WIDTH + j] = 0x000000;
						continue;
					} else if (pixel > 256) {
						mImage[i * IMG_WIDTH + j] = 0xFFFFFF;
						continue;
					}
					pixel = (pixel + 256) >> 1;
					if (pixel < 64) {
						mImage[i * IMG_WIDTH + j] = ((pixel * 4) << 16) | 0xFF;
					} else if (pixel < 128) {
						mImage[i * IMG_WIDTH + j] = 0xFF0000 | (0xFF - ((pixel - 64) * 4));
					} else if (pixel < 192) {
						mImage[i * IMG_WIDTH + j] = 0xFF0000 | ((pixel - 128) * 4) << 8;
					} else {
						mImage[i * IMG_WIDTH + j] = (0xFF - ((pixel - 192) * 4)) << 16 | 0xFF00;
					}
				}
			}
		}
	}
	emit newImage(mImage);

	for (auto i = 0u; i < mSensorHeight; ++i) {
		auto hCount = (IMG_HEIGHT / mSensorHeight);
		hCount += (i < IMG_HEIGHT % mSensorHeight) ? 1 : 0;
		for (auto j = 0u; j < mSensorWidth; ++j) {
			auto wCount = (IMG_WIDTH / mSensorWidth);
			wCount += (j < IMG_WIDTH % mSensorWidth) ? 1 : 0;
			auto avg = mSensorProcessBuffer[i * mSensorWidth + j] / (hCount * wCount);
			if (avg < -120) {
				mSensorData[i * mSensorWidth + j] = 0;
			} else if (avg < -80) {
				mSensorData[i * mSensorWidth + j] = 1;
			} else if (avg < -20) {
				mSensorData[i * mSensorWidth + j] = 2;
			} else if (avg < 20) {
				mSensorData[i * mSensorWidth + j] = 3;
			} else if (avg < 90) {
				mSensorData[i * mSensorWidth + j] = 4;
			} else if (avg < 200) {
				mSensorData[i * mSensorWidth + j] = 5;
			} else {
				mSensorData[i * mSensorWidth + j] = 6;
			}
			mSensorProcessBuffer[i * mSensorWidth + j] = 0;
		}
	}
	emit newSensorData(mSensorData);

	QTimer::singleShot(0, this, &IrCameraWorkerMLX90640::processFrame);
}

}
