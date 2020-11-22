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

#include <QEventLoop>
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
		, const trikHal::HardwareAbstractionInterface &hardwareAbstraction)
	: mState(state)
	, mHardwareAbstraction(hardwareAbstraction)
	, mEventFileName(eventFile)
	, mLastEventTimer(this)
	, mTryReopenTimer(this)
{
	mState.start();
}

void VectorSensorWorker::init()
{
	mEventFile.reset(mHardwareAbstraction.createEventFile(mEventFileName));

	mReading << 0 << 0 << 0 << 0 << 0 << 0;
	mReadingUnsynced = mReading;

	mLastEventTimer.setInterval(maxEventDelay);
	mLastEventTimer.setSingleShot(false);

	mTryReopenTimer.setInterval(reopenDelay);
	mTryReopenTimer.setSingleShot(false);
	connect(&mLastEventTimer, &QTimer::timeout, this, &VectorSensorWorker::onSensorHanged);
	connect(&mTryReopenTimer, &QTimer::timeout, this, &VectorSensorWorker::onTryReopen);

	connect(mEventFile.data(), &trikHal::EventFileInterface::newEvent, this, &VectorSensorWorker::onNewEvent);

	if (mEventFile->isOpened()) {
		mLastEventTimer.start();
	} else {
		QLOG_WARN() << "Sensor" << mState.deviceName() << ", device file can not be opened, will retry in"
				<< reopenDelay << "milliseconds";
		mTryReopenTimer.start();
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
		{
			QWriteLocker locker(&mReadingLock);
			mReading.swap(mReadingUnsynced);
			locker.unlock();
			emit newData(mReading, eventTime);
			break;
		}
		default:
			reportError();
	}
}

/// @todo: vector copying is not atomic, so we may receive evSyn right in the middle of "return mReading".
QVector<int> VectorSensorWorker::read()
{
	if (mState.isReady()) {
		QReadLocker locker(&mReadingLock);
		return mReading;
	} else {
		return {};
	}
}

void VectorSensorWorker::onSensorHanged()
{
	QLOG_WARN() << "Sensor" << mState.deviceName() << "hanged for " << maxEventDelay << "ms, reopening device file...";
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
