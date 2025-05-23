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
#include <trikKernel/timeVal.h>
#include <trikControl/trikControlDeclSpec.h>
#include "deviceInterface.h"




namespace trikControl {

/// Sensor that returns a vector.
class TRIKCONTROL_EXPORT VectorSensorInterface : public QObject, public DeviceInterface
{
	Q_OBJECT

Q_SIGNALS:
	/// Emitted when new sensor reading is ready.
	void newData(QVector<int> reading, const trikKernel::TimeVal &eventTime);

public Q_SLOTS:
	/// Returns current raw reading of a sensor.
	virtual QVector<int> read() const = 0;
};

}

Q_DECLARE_METATYPE(trikControl::VectorSensorInterface *)
