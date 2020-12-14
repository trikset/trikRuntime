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

#include <QtCore/QObject>
#include <QtCore/QString>
#include <QtCore/QScopedPointer>
#include <QReadWriteLock>

#include <trikHal/hardwareAbstractionInterface.h>
#include <trikKernel/timeVal.h>

#include "datafilter.h"
#include "deviceState.h"

namespace trikControl {

/// Worker object that processes range sensor output and updates stored reading. Meant to be executed in separate
/// thread.
class RangeSensorWorker : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param eventFile - event file for this sensor.
	RangeSensorWorker(const QString &eventFile, DeviceState &state
			, const trikHal::HardwareAbstractionInterface &hardwareAbstraction
			, int minValue, int maxValue, const QString &filterName);

	~RangeSensorWorker() override;

signals:
	/// Emitted when new data is received from a sensor.
	void newData(int distance, int rawDistance, const trikKernel::TimeVal &eventTime);

public slots:
	/// Initializes sensor and begins receiving events from it.
	void init();

	/// Returns current raw reading of a sensor.
	int read();

	/// Returns current real raw reading of a sensor.
	int readRawData();

	/// Stops sensor until init() will be called again.
	void stop();

private slots:
	/// Updates current reading when new value is ready in event file.
	void onNewEvent(int eventType, int code, int value, const trikKernel::TimeVal &eventTime);

private:
	/// Event file of a sensor driver.
	QScopedPointer<trikHal::EventFileInterface> mEventFile;

	int mDistance {-1};

	int mRawDistance {-1};

	/// State of a sensor, shared with proxy.
	DeviceState &mState;

	const trikHal::HardwareAbstractionInterface &mHardwareAbstraction;

	const QString mEventFileName;

	int mMinValue;
	int mMaxValue;
	QString mFilterName;
	QScopedPointer<DataFilter> mDataFilter;

	QReadWriteLock mDistanceLock;
};

}
