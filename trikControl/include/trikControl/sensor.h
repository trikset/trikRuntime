/* Copyright 2014 Roman Kurbatov
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

#include "declSpec.h"

namespace trikControl {

/// Abstract sensor class. It is inherited by AnalogSensor and DigitalSensor classes.
class TRIKCONTROL_EXPORT Sensor : public QObject
{
	Q_OBJECT

public:
	/// Destructor.
	virtual ~Sensor() {}

	enum Type {
		analogSensor
		, digitalSensor
		, specialSensor
	};

public slots:
	/// Returns current raw reading of a sensor.
	virtual int read() = 0;

	virtual int readRawData() = 0;
};

}
