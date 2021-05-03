/* Copyright 2018 Aleksey Fefelov and CyberTech Labs Ltd.
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

/// Class for work with i2c
class TRIKCONTROL_EXPORT I2cDeviceInterface : public QObject, public DeviceInterface
{
	Q_OBJECT

public :
	/// Sends data to current device, if it is connected.
	virtual void send(int reg, int value) = 0;

	/// Reads data by given I2C command number and returns the result.
	virtual int read(int reg) = 0;
};

}

Q_DECLARE_METATYPE(trikControl::I2cDeviceInterface *)
