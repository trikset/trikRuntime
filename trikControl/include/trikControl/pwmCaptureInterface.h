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

/// Provides characteristics of PWM signal supplied to the port.
class TRIKCONTROL_EXPORT PwmCaptureInterface : public QObject, public DeviceInterface
{
	Q_OBJECT

public:
	/// Gets minimal possible value for PWM capture reading received by duty() slot.
	virtual long minValue() const = 0;

	/// Gets maximal possible value for PWM capture reading received by duty() slot.
	virtual long maxValue() const = 0;

public slots:
	/// Returns three readings of PWM signal frequency.
	virtual QVector<int> frequency() = 0;

	/// Returns scaled PWM signal duty.
	virtual long duty() = 0;

	/// Returns raw PWM signal duty.
	virtual long dutyRaw() = 0;
};

}

Q_DECLARE_METATYPE(trikControl::PwmCaptureInterface *)
