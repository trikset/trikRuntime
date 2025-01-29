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

#include "eventDeviceWorker.h"

#include <QtCore/QThread>

#include <trikKernel/timeVal.h>

using namespace trikControl;

EventDeviceWorker::EventDeviceWorker(const QString &deviceFilePath, DeviceState &state
		, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mState(state)
	, mHardwareAbstraction(hardwareAbstraction)
	, mEventFileName(deviceFilePath)
{
	mState.start();
}

void EventDeviceWorker::init()
{
	mEventFile.reset(mHardwareAbstraction.createEventFile(mEventFileName));
	if (!mEventFile->open()) {
		mState.fail();
		return;
	}

	connect(mEventFile.data(), &trikHal::EventFileInterface::newEvent, this, &EventDeviceWorker::onNewEvent);
}

void EventDeviceWorker::onNewEvent(int eventType, int code, int value, const trikKernel::TimeVal &eventTime)
{
	Q_EMIT newEvent(eventType, code, value, eventTime.packedUInt32());
}
