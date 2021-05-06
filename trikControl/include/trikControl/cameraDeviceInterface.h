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

#include <QtCore/QVector>
#include <QImage>
#include <trikControl/trikControlDeclSpec.h>
#include "deviceInterface.h"

namespace trikControl {

/// Interface for camera device representation
class TRIKCONTROL_EXPORT CameraDeviceInterface : public DeviceInterface
{
public:

	/// Get photo as a vector of uint8t in RGB 888 format
	virtual QVector<uint8_t> getPhoto() = 0;

	virtual Status status() const override = 0;

	~CameraDeviceInterface() override = default;

	/// Convert QImage to QVector of uint8_t in RGB 888 format
	/// @param imgOrig - converting this QImage to QVector
	static QVector<uint8_t> qImageToQVector(const QImage &imgOrig);
};

}

Q_DECLARE_METATYPE(trikControl::CameraDeviceInterface *)
