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
#include <QtCore/QString>
#include <QtCore/QVector>

#include "deviceInterface.h"

#include <trikControl/trikControlDeclSpec.h>

namespace trikControl {

/// Uses virtual line sensor to detect x coordinate of a center of an object that was in camera's field of view
/// when "detect" method was called. Used mainly to follow the line.
class TRIKCONTROL_EXPORT LineSensorInterface : public QObject, public DeviceInterface
{
	Q_OBJECT

signals:
	/// Emitted when sensor is stopped successfully.
	void stopped();

public slots:
	/// Initializes a camera.
	/// @param showOnDisplay - true if we want an image from a camera to be drawn on robot display.
	virtual void init(bool showOnDisplay) = 0;

	/// Detects the color of an object in center of current frame and memorizes it.
	virtual void detect() = 0;

	/// Returns current raw x coordinate of detected object. Sensor returns 0 if detect() was not called.
	virtual QVector<int> read() = 0;

	/// Stops detection until init() will be called again.
	virtual void stop() = 0;

	/// Get values returned by last "detect" operation. Returned vector has 6 components - hue, saturation and value
	/// of a dominant color (got by "detect") and hue, saturation and value tolerance factors.
	virtual QVector<int> getDetectParameters() const = 0;
};

}

Q_DECLARE_METATYPE(trikControl::LineSensorInterface*)
