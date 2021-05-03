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
#include <QtMultimedia/QCamera>

#include "cameraImplementationInterface.h"
#include <trikControl/trikControlDeclSpec.h>

namespace trikControl {

/// Camera implementation with QtMultimedia
class TRIKCONTROL_EXPORT QtCameraImplementation : public CameraImplementationInterface
{
public:

	/// Qt camera constructor
	/// @param port - use this as name of device, i.e. "/dev/video0"
	explicit QtCameraImplementation(const QString & port);

	QVector<uint8_t> getPhoto() override;

	~QtCameraImplementation() override = default;

private:
	QScopedPointer<QCamera> mCamera;
};

}
