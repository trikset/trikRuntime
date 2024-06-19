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

#include <MLX90640_API.h>

#include <QtCore/QObject>
#include <QtCore/QVector>

#include "deviceState.h"

namespace trikControl {

class IrCameraWorkerMLX90640: public QObject
{
	Q_OBJECT

public:
	IrCameraWorkerMLX90640(uint8_t i2cAddr);
	~IrCameraWorkerMLX90640();

	/// Initialized i2c ir camera and starts frames capturing.
	void init();
	/// Stops frames capturing.
	void stop();

signals:
	/// Emitted when new frame is read and ready for processing.
	void newFrame(QVector<uint8_t>);
	/// Emitted when frames capturing successfully stopped.
	void stopped();


private slots:
	/// Reads new frame with raw data from IR camera
	void readBuffer();

private:

	/// Preprocessed frame
	QVector<uint8_t> mFrame;
	/// Buffer for read raw data
	QVector<uint16_t> mFrameBuffer;

	uint8_t mI2cAddr;
	DeviceState mState;
	uint8_t mRefreshRate = 0x3; // Corresponds to 4 Hz
	paramsMLX90640 mParams;
	static constexpr int EEPROM_SIZE = 832;
	static constexpr int FRAME_SIZE = 834;
	static constexpr int IMAGE_SIZE = 32 * 24;

};


} // namespace trikControl
