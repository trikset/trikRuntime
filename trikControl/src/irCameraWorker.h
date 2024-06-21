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

#pragma once

#include <cinttypes>
#include <MLX90640_API.h>

#include <QtCore/QObject>
#include <QtCore/QVector>

#include "deviceState.h"

namespace trikControl {

/// Class for ir camera background worker
class IrCameraWorkerMLX90640: public QObject
{
	Q_OBJECT

public:
	/// IrCamera background worker constructor.
	/// @param i2cAddr - I2C slave address of the ir camera
	/// @param m - number of rows in sensor grid
	/// @param n - number of columns in sensor grid
	IrCameraWorkerMLX90640(uint8_t i2cAddr, uint16_t m, uint16_t n);

	~IrCameraWorkerMLX90640();

	/// Initialized i2c ir camera and starts frames capturing.
	void init();

	/// Stops frames capturing.
	void stop();

signals:
	/// Emitted when new image provided by worker.
	void newImage(QVector<int32_t>);

	/// Emitted when new sensor grid provided by worker.
	void newSensorData(QVector<int32_t>);

	/// Emitted when frames capturing successfully stopped.
	void stopped();

private slots:
	/// Reads new frame with raw data from IR camera.
	void processFrame();

private:
	/// I2C slave address of the camera
	uint8_t mI2cAddr;

	/// Refresh rate of the MLX90640.
	/// - 0x00 – 0.5Hz
	/// - 0x01 – 1Hz
	/// - 0x02 – 2Hz
	/// - 0x03 – 4Hz (maximum frequency now)
	/// - 0x04 – 8Hz
	/// - 0x05 – 16Hz
	/// - 0x06 – 32Hz
	/// - 0x07 – 64Hz
	uint8_t mRefreshRate = 0x3;

	/// Number of rows in sensor grid
	uint16_t mSensorHeight;

	/// Number of columns in sensor grid
	uint16_t mSensorWidth;

	/// Buffer for read raw data
	QVector<uint16_t> mFrameBuffer;

	/// Buffer for processed image
	QVector<int32_t> mImage;

	/// Buffer with frame data preprocessed for sensor
	QVector<int32_t> mSensorProcessBuffer;

	/// Buffer (mSensorHeight x mSensorWidth) with sensor data
	QVector<int32_t> mSensorData;

	/// Internal parameters of the camera
	paramsMLX90640 mParams;

	/// State of the worker
	DeviceState mState;

	/// Size of eeprom in words
	static constexpr int EEPROM_SIZE = 832;

	/// Size of frame data in words
	static constexpr int FRAME_SIZE = 834;

	/// Number of rows in image
	static constexpr int IMG_HEIGHT = 24;

	/// Number of columns in image
	static constexpr int IMG_WIDTH = 32;

	/// Size of image in pixels
	static constexpr int IMG_SIZE = IMG_HEIGHT * IMG_WIDTH;
};

} // namespace trikControl
