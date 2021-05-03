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

/// Uses virtual sound sensor to detect angle of sound source
class TRIKCONTROL_EXPORT SoundSensorInterface : public QObject, public DeviceInterface
{
	Q_OBJECT

signals:
	/// Emitted when sensor is stopped successfully.
	void stopped();

public slots:
	/// Initializes an ALSA
	/// @param showOnDisplay - true if we want an image from DSP to be drawn on robot display.
	virtual void init(bool showOnDisplay) = 0;

	/// Not use
	virtual void detect() = 0;

	/// Sets volume coefficient
	virtual void volume(int volCoeff) = 0;

	/// Returns angle and volume for both channels
	virtual QVector<int> read() = 0;

	/// Stops detection until init() will be called again.
	virtual void stop() = 0;
};

}

Q_DECLARE_METATYPE(trikControl::SoundSensorInterface*)
