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

#include <QtCore/QObject>
#include <QtCore/QVector>
#include <QImage>
#include <trikControl/trikControlDeclSpec.h>
#include "deviceInterface.h"

namespace trikControl {

/// Interface for camera device representation
class TRIKCONTROL_EXPORT IrCameraInterface : public QObject, public DeviceInterface
{
	Q_OBJECT

public:
	~IrCameraInterface() override = default;

public Q_SLOTS:
	/// Initializes an ir camera and starts frames capturing.
	virtual void init() = 0;

	/// Stops frames capturing until init() will be called again.
	virtual void stop() = 0;

	/// Gets photo as a vector of uint8_t in Grayscale8 format.
	virtual QVector<int32_t> getImage() = 0;

	/// Returns average value in given cell of a grid.
	/// @param[in] m row number
	/// @param[in] n column number
	virtual int8_t readSensor(int m, int n) = 0;

	Status status() const override = 0;

signals:
	/// Emitted when new frame is ready for processing.
	void newFrame();
	/// Emitted when camera is stopped successfully.
	void stopped();

};

}

Q_DECLARE_METATYPE(trikControl::IrCameraInterface *)
