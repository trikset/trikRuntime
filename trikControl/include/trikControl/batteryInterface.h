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

#include "declSpec.h"

namespace trikControl {

/// Provides battery voltage info.
class TRIKCONTROL_EXPORT BatteryInterface : public QObject, public DeviceInterface
{
	Q_OBJECT
public slots:
	/// Returns current battery voltage in volts.
	virtual float readVoltage() = 0;

	/// Returns current raw reading of battery.
	virtual float readRawDataVoltage() = 0;
};

}
