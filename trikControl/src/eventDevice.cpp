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

#include "eventDevice.h"

#include <QsLog.h>

#include "eventDeviceWorker.h"
#include "event.h"

using namespace trikControl;

EventDevice::EventDevice(const QString &eventFile, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mState("EventDevide: " + eventFile)
{
	mWorker.reset(new EventDeviceWorker(eventFile, mState, hardwareAbstraction));
	if (!mState.isFailed()) {
		connect(mWorker.data(), SIGNAL(newEvent(int, int, int, int)), this, SIGNAL(on(int, int, int, int))
				, Qt::QueuedConnection);

		mWorker->moveToThread(&mWorkerThread);

		QLOG_INFO() << "Starting generic event device" << eventFile << " worker thread" << &mWorkerThread;

		mWorkerThread.start();
		mState.ready();
	}
}

EventDevice::~EventDevice()
{
	if (mWorkerThread.isRunning()) {
		mWorkerThread.quit();
		mWorkerThread.wait();
	}
}

EventInterface *EventDevice::onEvent(int eventType)
{
	if (!mEvents.contains(eventType)) {
		const QSharedPointer<Event> event(new Event(eventType));
		connect(this, SIGNAL(on(int, int, int, int)), event.data(), SLOT(onEvent(int, int, int, int)));
		mEvents.insert(eventType, event);
	}

	return mEvents.value(eventType).data();
}

EventDevice::Status EventDevice::status() const
{
	return mState.status();
}
