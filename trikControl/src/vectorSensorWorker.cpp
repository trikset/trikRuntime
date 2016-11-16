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

static const int maxEventDelay = 1000;
static const int reopenDelay = 1000;

static const int evSyn = 0;
static const int evAbs = 3;
static const int absX = 0x0;
static const int absY = 0x01;
static const int absZ = 0x02;

using namespace trikControl;

VectorSensorWorker::VectorSensorWorker(const QString &eventFile, DeviceState &state
		, const trikHal::HardwareAbstractionInterface &hardwareAbstraction
		, QThread &thread)
	: mEventFile(hardwareAbstraction.createEventFile(eventFile, thread))
	, mState(state)
{
	mState.start();

	mReading << 0 << 0 << 0 << 0 << 0 << 0;
	mReadingUnsynced = mReading;

	moveToThread(&thread);

	mLastEventTimer.moveToThread(&thread);
	mLastEventTimer.setInterval(maxEventDelay);
	mLastEventTimer.setSingleShot(false);

	mTryReopenTimer.moveToThread(&thread);
	mTryReopenTimer.setInterval(reopenDelay);
	mTryReopenTimer.setSingleShot(false);

	connect(mEventFile.data(), SIGNAL(newEvent(int, int, int, trikKernel::TimeVal))
			, this, SLOT(onNewEvent(int, int, int, trikKernel::TimeVal)));

	connect(&mLastEventTimer, SIGNAL(timeout()), this, SLOT(onSensorHanged()));
	connect(&mTryReopenTimer, SIGNAL(timeout()), this, SLOT(onTryReopen()));

	mEventFile->open();
	thread.start();

	if (mEventFile->isOpened()) {
		// Timer should be started in its thread, so doing it via metacall
		QMetaObject::invokeMethod(&mLastEventTimer, "start");
	} else {
		QLOG_WARN() << "Sensor" << mState.deviceName() << ", device file can not be opened, will retry in"
				<< reopenDelay << "milliseconds";
		// Timer should be started in its thread, so doing it via metacall
		QMetaObject::invokeMethod(&mTryReopenTimer, "start");
		mState.fail();
	}
}

void VectorSensorWorker::onNewEvent(int eventType, int code, int value, const trikKernel::TimeVal &eventTime)
{
	mLastEventTimer.start();

	if (mState.isFailed()) {
		mState.resetFailure();
		mState.ready();
	}

	const auto reportError = [&](){
		QLOG_ERROR() << "Unknown event type in vector sensor event file" << mEventFile->fileName() << " :"
				<< eventType << code << value;
	};

	switch (eventType) {
		case evAbs:
			switch (code) {
			case absX:
				mReadingUnsynced[0] = value;
				break;
			case absY:
				mReadingUnsynced[1] = value;
				break;
			case absZ:
				mReadingUnsynced[2] = value;
				break;
			default:
				reportError();
			}
			break;
		case evSyn:
			mReading.swap(mReadingUnsynced);
			emit newData(mReading, eventTime);
			break;
		default:
			reportError();
	}
}

/// @todo: vector copying is not atomic, so we may receive evSyn right in the middle of "return mReading".
QVector<int> VectorSensorWorker::read()
{
	if (mState.isReady()) {
		return mReading;
	} else {
		return {};
	}
}

void VectorSensorWorker::deinitialize()
{
	mLastEventTimer.stop();
	mTryReopenTimer.stop();
}

void VectorSensorWorker::onSensorHanged()
{
	QLOG_WARN() << "Sensor" << mState.deviceName() << "hanged, reopening device file...";
	mState.fail();
	mLastEventTimer.stop();

	mEventFile->close();
	mEventFile->open();

	if (!mEventFile->isOpened()) {
		mTryReopenTimer.start();
	} else {
		QLOG_INFO() << "Sensor" << mState.deviceName() << ", device file reopened.";
		mLastEventTimer.start();
		mTryReopenTimer.stop();
	}
}

void VectorSensorWorker::onTryReopen()
{
	onSensorHanged();
}
