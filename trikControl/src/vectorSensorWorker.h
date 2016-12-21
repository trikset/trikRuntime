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

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QVector>
#include <QtCore/QTimer>
#include <QtCore/QReadWriteLock>
#include <QtCore/QThread>

#include <trikHal/hardwareAbstractionInterface.h>

#include "deviceState.h"

namespace trikKernel {
class TimeVal;
}

namespace trikControl {

/// Handles events from sensor, intended to work in separate thread.
class VectorSensorWorker : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param eventFile - device file for this sensor.
	/// @param state - state of a device.
	/// @param thread - background thread where all socket events will be processed.
	VectorSensorWorker(const QString &eventFile, DeviceState &state
			, const trikHal::HardwareAbstractionInterface &hardwareAbstraction
			, QThread &thread);

signals:
	/// Emitted when new sensor reading is ready.
	void newData(QVector<int> reading, const trikKernel::TimeVal &eventTime);

public slots:
	/// Returns current raw reading of a sensor.
	QVector<int> read();

	/// Shuts down sensor.
	void deinitialize();

private slots:
	/// Updates current reading when new value is ready in event file.
	void onNewEvent(int eventType, int code, int value, const trikKernel::TimeVal &eventTime);

	/// Called when there are no events from event file for too long (1 second hardcoded). Attempts to reopen
	/// event file.
	void onSensorHanged();

	/// Called when timeout for another try to reestablish connection with a device is expired. Tries to reopen
	/// event file.
	void onTryReopen();

private:
	/// Event file for that sensor.
	QScopedPointer<trikHal::EventFileInterface> mEventFile;

	/// Current reading that will be returned on read() call.
	QVector<int> mReading;

	/// Current partial reading, will be copied to mReading on SYNC signal.
	QVector<int> mReadingUnsynced;

	/// Device state, shared between worker and proxy.
	DeviceState &mState;

	/// Timer that reopens event file when there are no events for too long (1 second hardcoded).
	QTimer mLastEventTimer;

	/// Timer that initiates attempts to reopen device file if there is a hangup.
	QTimer mTryReopenTimer;
};

}
