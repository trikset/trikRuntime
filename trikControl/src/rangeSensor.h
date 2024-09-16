/* Copyright 2014 - 2015 CyberTech Labs Ltd.
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

#include "sensorInterface.h"
#include "deviceState.h"

namespace trikKernel {
class TimeVal;
}

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikKernel {
class Configurer;
}

namespace trikControl {

class RangeSensorWorker;
class ModuleLoader;

/// TRIK range sensor.
class RangeSensor : public SensorInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param port - port on which this sensor is configured.
	/// @param configurer - configurer object containing preparsed XML files with sensor parameters.
	RangeSensor(const QString &port, const trikKernel::Configurer &configurer, ModuleLoader &moduleLoader
		, const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	~RangeSensor() override;

	Status status() const override;

	int minValue() const override;

	int maxValue() const override;

signals:
	/// Emitted when new data is received from a sensor.
	void newData(int distance, int rawDistance, const trikKernel::TimeVal &eventTime);

public slots:
	/// Initializes sensor and begins receiving events from it.
	void init();

	/// Returns current raw reading of a sensor.
	int read() override;

	/// Returns current real raw reading of a sensor.
	int readRawData() override;

	/// Stops sensor until init() will be called again.
	void stop();

private:
	/// Device state, shared with worker.
	DeviceState mState;

	/// Worker object that handles sensor in separate thread.
	QScopedPointer<RangeSensorWorker> mSensorWorker;

	/// Worker thread.
	QThread mWorkerThread;

	/// Minimal possible normalized value returned by sensor.
	int mMinValue = 0;

	/// Maximal possible normalized value returned by sensor.
	int mMaxValue = 0;
};

}
