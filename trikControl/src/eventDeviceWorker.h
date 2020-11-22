/* Copyright 2016 CyberTech Labs Ltd.
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

#include <trikHal/hardwareAbstractionInterface.h>

#include "deviceState.h"

namespace trikControl {

/// Worker for generic event device that uses HAL device file to listen to events. Meant to be run in separate thread.
class EventDeviceWorker : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param deviceFilePath - path to device file that will provide events.
	/// @param state - device state, worker sets it to "failed" if it can not open file.
	/// @param hardwareAbstraction - interface to TRIK hardware.
	EventDeviceWorker(const QString &deviceFilePath, DeviceState &state
			, const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	/// Execute init() after worker thread started.
	void init();

signals:
	/// Emitted when there is new event in an event file.
	/// @param event - type of the event.
	/// @param code - code of the event inside a type.
	/// @param value - value sent with the event.
	/// @param eventTime - time stamp of the event, in msecs form start of the epoch.
	void newEvent(int onEvent, int code, int value, int eventTime);

private slots:
	/// Called every time underlying event file produces an event.
	void onNewEvent(int eventType, int code, int value, const trikKernel::TimeVal &eventTime);

private:
	/// Underlying event file that watches actual event file from operating system.
	QScopedPointer<trikHal::EventFileInterface> mEventFile;

	DeviceState &mState;
	const trikHal::HardwareAbstractionInterface &mHardwareAbstraction;
	const QString mEventFileName;
};

}
