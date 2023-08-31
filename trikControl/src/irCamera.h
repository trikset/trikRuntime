/* Copyright 2023 Nick Ponomarev
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

// #include <QtCore/QScopedPointer>
#include <QtCore/QVector>

// #include <QMutex>

#include "irCameraInterface.h"
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
public:

	/// IrCamera constructor
	/// @param mediaPath - path where program should save photos
	/// @param configurer - configurer to get info from config
	/// @param hardwareAbstraction - realization of HAL
	IrCamera(const QString &port
				 , const trikKernel::Configurer &configurer
				 , trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	QVector<int32_t> getImage() override;

	Status status() const override;

	~IrCamera() override = default;

private:
	uint8_t mI2cAddr;
	DeviceState mState;
	uint8_t mRefreshRate = 0x3; // Corresponds to 4 Hz
	paramsMLX90640 mParams;
	static constexpr int EEPROM_SIZE = 832;
	static constexpr int FRAME_SIZE = 834;
	static constexpr int IMAGE_SIZE = 768;
//	QMutex mCameraMutex;
};

}
