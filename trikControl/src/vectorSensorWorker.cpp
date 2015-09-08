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

#include "src/vectorSensorWorker.h"

#include <QsLog.h>

using namespace trikControl;

VectorSensorWorker::VectorSensorWorker(const QString &eventFile, DeviceState &state
		, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mEventFile(hardwareAbstraction.createEventFile(eventFile))
	, mState(state)
{
	mReading << 0 << 0 << 0;
	mReadingUnsynced = mReading;

	mState.start();

	connect(mEventFile.data(), SIGNAL(newEvent(trikHal::EventFileInterface::EventType, int, int))
			, this, SLOT(onNewEvent(trikHal::EventFileInterface::EventType, int, int)));

	mEventFile->open();
}

void VectorSensorWorker::onNewEvent(trikHal::EventFileInterface::EventType eventType, int code, int value)
{
	switch (eventType) {
		case trikHal::EventFileInterface::EventType::evAbsX:
			mReadingUnsynced[0] = value;
			break;
		case trikHal::EventFileInterface::EventType::evAbsY:
			mReadingUnsynced[1] = value;
			break;
		case trikHal::EventFileInterface::EventType::evAbsZ:
			mReadingUnsynced[2] = value;
			break;
		case trikHal::EventFileInterface::EventType::evSyn:
			mReading.swap(mReadingUnsynced);
			emit newData(mReading);
			break;
		default:
			QLOG_ERROR() << "Unknown event type in vector sensor event file"<< mEventFile->fileName() << " :"
					<< static_cast<int>(eventType) << code << value;
	}
}

QVector<int> VectorSensorWorker::read()
{
	if (mState.isReady()) {
		return mReading;
	} else {
		return {};
	}
}
