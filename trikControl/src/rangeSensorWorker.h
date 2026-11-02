/* Copyright 2014 - 2016 CyberTech Labs Ltd.
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

#include <QtCore/QElapsedTimer>
#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QReadWriteLock>

#include <trikKernel/timeVal.h>

#include "datafilter.h"
#include "deviceState.h"

namespace trikHal {
class HardwareAbstractionInterface;
class InputDeviceFileInterface;
}

namespace trikControl {

/// Worker object that processes range sensor output and updates stored reading. Meant to be executed in separate
/// thread.
class RangeSensorWorker : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param hardwareAbstraction - hardware abstraction used to create the input device file.
	/// @param sensorFile - path to the IIO sysfs file with distance value (in_distance_raw).
	/// @param samplingPeriodMs - how often the sensor file is polled, in milliseconds.
	RangeSensorWorker(const trikHal::HardwareAbstractionInterface &hardwareAbstraction, QString sensorFile,
		DeviceState &state, int minValue, int maxValue, QString filterName, int samplingPeriodMs);

	~RangeSensorWorker() override;

Q_SIGNALS:
	/// Emitted when new data is received from a sensor.
	void newData(int distance, int rawDistance, const trikKernel::TimeVal &eventTime);

public Q_SLOTS:
	/// Initializes sensor and begins receiving readings from it.
	void init();

	/// Returns current raw reading of a sensor.
	int read();

	/// Returns current real raw reading of a sensor.
	int readRawData();

	/// Stops sensor until init() will be called again.
	void stop();

private Q_SLOTS:
	/// Reads value from sysfs and updates current reading.
	void onTimer();

private:
	/// IIO sysfs file this sensor reads from.
	QScopedPointer<trikHal::InputDeviceFileInterface> mDistanceFile;

	/// Timer that triggers periodic sysfs read.
	QScopedPointer<QTimer> mReadTimer;

	/// How often the sensor file is polled, in milliseconds.
	int mSamplingPeriodMs;

	/// Monotonic timer used for event timestamps.
	QElapsedTimer mElapsedTimer;

	int mDistance {-1};

	int mRawDistance {-1};

	/// Hardware abstraction used to create the input device file.
	const trikHal::HardwareAbstractionInterface &mHardwareAbstraction;

	/// State of a sensor, shared with proxy.
	DeviceState &mState;

	const QString mSensorFile;

	int mMinValue;
	int mMaxValue;
	QString mFilterName;
	QScopedPointer<DataFilter> mDataFilter;

	QReadWriteLock mDistanceLock;
};

}
