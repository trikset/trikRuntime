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

#include "src/keysWorker.h"

#include <QsLog.h>

using namespace trikControl;

KeysWorker::KeysWorker(const QString &keysPath, DeviceState &state
		, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mState(state)
{
	mState.start();
	mEventFile.reset(hardwareAbstraction.createEventFile());
	if (!mEventFile->open(keysPath)) {
		mState.fail();
		return;
	}

	connect(mEventFile.data(), SIGNAL(newEvent(trikHal::EventFileInterface::EventType, int, int))
			, this, SLOT(readKeysEvent(trikHal::EventFileInterface::EventType, int, int)));
}

void KeysWorker::reset()
{
	mLock.lockForWrite();
	mWasPressed.clear();
	mLock.unlock();
}

bool KeysWorker::wasPressed(int code)
{
	mLock.lockForRead();
	const bool result = mWasPressed.contains(code);
	mLock.unlock();

	if (result) {
		mLock.lockForWrite();
		mWasPressed.remove(code);
		mLock.unlock();
	}

	return result;
}

void KeysWorker::readKeysEvent(trikHal::EventFileInterface::EventType eventType, int code, int value)
{
	switch (eventType)
	{
	case trikHal::EventFileInterface::EventType::evKey:
		mButtonCode = code;
		mButtonValue = value;
		break;
	case trikHal::EventFileInterface::EventType::evSyn:
		if (mButtonValue) {
			mLock.lockForWrite();
			mWasPressed.insert(mButtonCode);
			mLock.unlock();
		}

		emit buttonPressed(mButtonCode, mButtonValue);
		break;
	default:
		QLOG_ERROR() << "Event of unknown type in keys device file";
		break;
	}
}
