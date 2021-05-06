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

#include "deviceInterface.h"

#include <trikControl/trikControlDeclSpec.h>

namespace trikControl {

/// Class for handling keys on a brick.
class TRIKCONTROL_EXPORT KeysInterface : public QObject, public DeviceInterface
{
	Q_OBJECT

public slots:
	/// Clear data about previous key pressures.
	virtual void reset() = 0;

	/// Returns true if button with given code was pressed, and clears "pressed" state for that button.
	virtual bool wasPressed(int code) = 0;

	/// Returns true if button with given code is pressed at the moment.
	virtual bool isPressed(int code) = 0;

	/// Returns a code of a pressed button.
	virtual int buttonCode(bool wait = true) = 0;

signals:
	/// Triggered when button state changed (pressed or released).
	/// @param code - key code.
	/// @param value - key state.
	void buttonPressed(int code, int value);
};

}

Q_DECLARE_METATYPE(trikControl::KeysInterface *)
