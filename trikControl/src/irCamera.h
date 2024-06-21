/* Copyright 2023-2024 Nick Ponomarev, Vladimir Kutuev
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

#include <QtCore/QScopedPointer>
#include <QtCore/QThread>
#include <QtCore/QVector>


#include "irCameraInterface.h"
#include "irCameraWorker.h"
#include "deviceState.h"
#include <trikControl/trikControlDeclSpec.h>
#include <MLX90640_API.h>

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

/// Class for ir camera device instantiation
class TRIKCONTROL_EXPORT IrCamera : public IrCameraInterface
{
	Q_OBJECT

public:
	/// IrCamera constructor.
	/// @param mediaPath - path where program should save photos
	/// @param configurer - configurer to get info from config
	/// @param hardwareAbstraction - realization of HAL
	IrCamera(const QString &port
			 , const trikKernel::Configurer &configurer
			 , trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	~IrCamera() override;

public Q_SLOTS:
	void init() override;

	void stop() override;

	QVector<int32_t> getImage() const override;

	int readSensor(int m, int n) const override;

	Status status() const override;

private slots:
	/// Process new image from IrCameraWorker.
	void onNewImage(QVector<int32_t> image);

	/// Process new sensor data from IrCameraWorker.
	void onNewSensorData(QVector<int32_t> grid);

	/// Process IrCameraWorker stopped signal.
	void onStop();

private:
	/// Worker object that read data from ir camera in separate thread
	QScopedPointer<IrCameraWorkerMLX90640> mIrCameraWorker;

	/// Worker thread for frames capturing and processing
	QThread mWorkerThread;

	/// Colored image got from worker
	QVector<int32_t> mImage;

	/// Sensor grid got from worker
	QVector<int32_t> mSensorData;

	/// Height of the sensor grid
	uint16_t mSensorHeight;

	/// Width of the sensor grid
	uint16_t mSensorWidth;

	/// State of the Ir Camera
	DeviceState mState;

	/// Number of columns in image
	static constexpr int IMAGE_WIDTH = 32;

	/// Number of rows in image
	static constexpr int IMAGE_HEIGHT = 24;

	/// Size of image in pixels
	static constexpr int IMAGE_SIZE = IMAGE_WIDTH * IMAGE_HEIGHT;
};

}
