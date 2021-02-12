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

#include <QtCore/QThread>
#include <QsLog.h>

using namespace trikControl;

static const int evSyn = 0;
static const int evKey = 1;

KeysWorker::KeysWorker(const QString &keysPath, DeviceState &state
		, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mHardwareAbstraction(hardwareAbstraction)
	, mKeysPath(keysPath)
	, mState(state)
{
	mState.start();
}

void KeysWorker::init()
{
	mEventFile.reset(mHardwareAbstraction.createEventFile(mKeysPath));
	if (!mEventFile->open()) {
		mState.fail();
		return;
	}

	connect(mEventFile.data(), &trikHal::EventFileInterface::newEvent, this, &KeysWorker::readKeysEvent);
}

void KeysWorker::reset()
{
	mLock.lockForWrite();
	emit stopWaiting();
	mWasPressed.clear();
	mButtonCode = 0;
	mButtonValue = 0;
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

void KeysWorker::readKeysEvent(int eventType, int code, int value
		, const trikKernel::TimeVal &eventTime)
{
	Q_UNUSED(eventTime);

	switch (eventType)
	{
	case evKey:
		mButtonCode = code;
		mButtonValue = value;
		break;
	case evSyn:
		if (mButtonCode && mButtonValue) {
			mLock.lockForWrite();
			mWasPressed.insert(mButtonCode);
			mLock.unlock();
		}

		emit buttonPressed(mButtonCode, mButtonValue);

		mButtonCode = 0;
		mButtonValue = 0;
		break;
	default:
		QLOG_ERROR() << "Event of unknown type in keys device file";
		break;
	}
}
