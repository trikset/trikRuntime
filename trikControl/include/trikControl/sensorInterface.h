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

/// Abstract sensor, inherited by various sensor types.
class TRIKCONTROL_EXPORT SensorInterface : public QObject, public DeviceInterface
{
	Q_OBJECT

public:
	enum class Type {
		analogSensor
		, digitalSensor
		, specialSensor
	};

	/// Gets minimal possible value for sensor reading received by read() slot.
	virtual int minValue() const = 0;

	/// Gets maximal possible value for sensor reading received by read() slot.
	virtual int maxValue() const = 0;

public slots:
	/// Returns current reading of a sensor.
	virtual int read() = 0;

	/// Returns current raw reading of a sensor.
	virtual int readRawData() = 0;
};

}

Q_DECLARE_METATYPE(trikControl::SensorInterface *)
