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

#include "cameraImplementationInterface.h"
#include <trikHal/hardwareAbstractionInterface.h>
#include <trikControl/trikControlDeclSpec.h>


namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

/// Camera implementation using v4l2
class V4l2CameraImplementation : public CameraImplementationInterface
{
public:

	/// V4l2 camera constructor
	/// @param port - name of device, i.e. "/dev/video0"
	/// @param hardwareAbstraction - realization of HAL
	V4l2CameraImplementation(const QString &port, trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	QVector<uint8_t> getPhoto() override;

	~V4l2CameraImplementation() override = default;
private:
	trikHal::HardwareAbstractionInterface & mHal;
	QString mPort;
};

}
