/* Copyright 2021 CyberTech Labs Ltd.
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

#include "vectorSensorInterface.h"

#include <trikControl/trikControlDeclSpec.h>

namespace trikControl {

/// Lidar sensor
class TRIKCONTROL_EXPORT LidarInterface : public VectorSensorInterface
{
	Q_OBJECT

	/// Returns current raw reading of a lidar.
	virtual QVector<int> readRaw() const = 0;
};
}

Q_DECLARE_METATYPE(trikControl::LidarInterface *)
