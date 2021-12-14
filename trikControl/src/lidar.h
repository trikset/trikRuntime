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

#include <QtCore/QThread>
#include <QtCore/QScopedPointer>

#include "lidarInterface.h"
#include "deviceState.h"
#include "lidarWorker.h"

#include <trikControl/trikControlDeclSpec.h>

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

/// Implementation of lidar for real robot.
class Lidar : public LidarInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - port on which this sensor is configured.
	/// @param configurer - configurer object containing preparsed XML files with lidar parameters.
	Lidar(const QString &port, const trikKernel::Configurer &configurer
			, trikHal::HardwareAbstractionInterface &hardwareAbstraction);
	~Lidar() override;

	Status status() const override;

	Q_INVOKABLE QVector<int> read() const override;

	Q_INVOKABLE QVector<int> readRaw() const override;

private:
	LidarWorker *mLidarWorker; // Has ownership

	/// Worker thread.
	QThread mWorkerThread;
};

}
