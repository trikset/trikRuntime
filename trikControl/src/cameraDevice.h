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

#pragma once

#include <QtCore/QScopedPointer>
#include <QtCore/QVector>

#include <QMutex>

#include "cameraDeviceInterface.h"
#include "cameraImplementationInterface.h"
#include <trikControl/trikControlDeclSpec.h>

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

/// Class for camera device instantiation
class TRIKCONTROL_EXPORT CameraDevice : public CameraDeviceInterface
{
public:

	/// CameraDevice constructor
	/// @param mediaPath - path where program should save photos
	/// @param configurer - configurer to get info from config
	/// @param hardwareAbstraction - realization of HAL
	CameraDevice(const QString &port, const QString & mediaPath
				 , const trikKernel::Configurer &configurer
				 , trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	QVector<uint8_t> getPhoto() override;

	Status status() const override;

	~CameraDevice() override = default;

private:
	QMutex mCameraMutex;
	QScopedPointer<CameraImplementationInterface> mCameraImpl;
};

}
