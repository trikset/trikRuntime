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
class VectorSensorWorkerIIO : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param iioDevFile - device file for this sensor.
	/// @param state - state of a device.
	/// @param thread - background thread where all socket events will be processed.
	VectorSensorWorkerIIO(const QString &iioDevFile, DeviceState &state
			, const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

signals:
	/// Emitted when new sensor reading is ready.
	void newData(QVector<int> reading, const trikKernel::TimeVal &eventTime);

public slots:
	/// Returns current raw reading of a sensor.
	QVector<int> read();

	/// Shuts down sensor.
	void deinitialize();

	void init();

private:
	/// Event file for that sensor.
	QScopedPointer<trikHal::IIOFileInterface> mIIOFile;

	/// Current reading that will be returned on read() call.
	QVector<int> mReading;

	/// Device state, shared between worker and proxy.
	DeviceState &mState;
};

}
