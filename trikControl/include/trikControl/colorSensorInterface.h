/* Copyright 2015 CyberTech Labs Ltd.
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

#include "deviceInterface.h"

#include <trikControl/trikControlDeclSpec.h>

namespace trikControl {

/// Uses virtual color sensor to detect dominant color in every cell of m * n grid of camera field of view.
class TRIKCONTROL_EXPORT ColorSensorInterface : public QObject, public DeviceInterface
{
	Q_OBJECT

signals:
	/// Emitted when sensor is stopped successfully.
	void stopped();

public slots:
	/// Initializes a camera.
	/// @param showOnDisplay - true if we want an image from a camera to be drawn on robot display.
	virtual void init(bool showOnDisplay) = 0;

	/// Returns dominant color in given cell of a grid as a vector [R; G; B] in RGB color scale.
	virtual QVector<int> read(int m, int n) = 0;

	/// Stops detection until init() will be called again.
	virtual void stop() = 0;
};

}

Q_DECLARE_METATYPE(trikControl::ColorSensorInterface *)
