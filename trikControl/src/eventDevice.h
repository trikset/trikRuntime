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

#include <QtCore/QSharedPointer>
#include <QThread>

#include "eventDeviceInterface.h"
#include "deviceState.h"

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

class EventDeviceWorker;

/// Implementation of generic event device that uses HAL device file to provide events.
class EventDevice : public EventDeviceInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param eventFile - path to event file that this object needs to watch.
	/// @param hardwareAbstraction - interface to underlying hardware or operating system capabilities of a robot.
	EventDevice(const QString &eventFile, const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	~EventDevice() override;

	EventInterface *onEvent(int eventType) override;

	Status status() const override;

private:
	/// Actual implementation of event device watcher in separate thread (in case there are too many events).
	QScopedPointer<EventDeviceWorker> mWorker;

	/// Thread for event file watcher.
	QThread mWorkerThread;

	/// State of event device.
	DeviceState mState;

	/// A storage for created event objects. They will listen to the events from the moment of first reference
	/// until event device is shut down.
	QHash<int, QSharedPointer<EventInterface>> mEvents;
};

}
