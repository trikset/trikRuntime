/* Copyright 2014 CyberTech Labs Ltd.
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

#include "src/rangeSensorWorker.h"

#include <QtCore/QThread>
#include <QsLog.h>

static const int evSyn = 0;
static const int evAbs = 3;
static const int absDistance = 0x19;
static const int absMisc = 0x28;

using namespace trikControl;

RangeSensorWorker::RangeSensorWorker(const QString &eventFile, DeviceState &state
		, const trikHal::HardwareAbstractionInterface &hardwareAbstraction
		, int minValue, int maxValue, const QString &filterName)
	: mState(state)
	, mHardwareAbstraction(hardwareAbstraction)
	, mEventFileName(eventFile)
	, mMinValue(minValue)
	, mMaxValue(maxValue)
	, mFilterName(filterName)
{
	mState.start();
}

RangeSensorWorker::~RangeSensorWorker()
{
	if (mState.isReady()) {
		stop();
	}
}

void RangeSensorWorker::stop()
{
	if (mState.isReady()) {
		mState.stop();
		if (mEventFile->close()) {
			mState.off();
		} else {
			mState.fail();
		}
	} else if (mState.status() == DeviceInterface::Status::starting) {
		// Maybe event file still waits for a driver to create event file, abort waiting.
		mEventFile->cancelWaiting();

		// In case of file is already opened --- close it. On unopened file "close" will do nothing.
		mEventFile->close();

		// Officially shut down sensor.
		mState.ready();
		mState.off();
	} else {
		QLOG_ERROR() << "Trying to stop range sensor that is not started, ignoring";
	}
}

void RangeSensorWorker::init()
{
	mEventFile.reset(mHardwareAbstraction.createEventFile(mEventFileName));

	connect(mEventFile.data(), &trikHal::EventFileInterface::newEvent, this, &RangeSensorWorker::onNewEvent);

	if (mEventFile->open()) {
		mState.ready();
	} else {
		if (mState.status() == DeviceInterface::Status::off) {
			// Launching was aborted, do nothing.
			return;
		}

		// Sensor launch failed for some reason, assuming permanent failure.
		mState.fail();
	}

	if (mFilterName == "median3") {
		mDataFilter.reset(new DataFilter(mMinValue, mMaxValue, "median3"));
	}
}

void RangeSensorWorker::onNewEvent(int eventType, int code, int value, const trikKernel::TimeVal &eventTime)
{
	if (!mState.isReady()) {
		return;
	}

	QWriteLocker locker(&mDistanceLock);
	switch (eventType) {
	case evAbs:
		switch (code) {
		case absDistance:
			mDistance = !mDataFilter ? value : mDataFilter->applyFilter(value);
			break;
		case absMisc:
			mRawDistance = value;
			break;
		default:
			QLOG_ERROR() << "Unknown event in range sensor event file:" << eventType << code << value;
		}
		break;
	case evSyn:
		locker.unlock();
		emit newData(mDistance, mRawDistance, eventTime);
		break;
	default:
		QLOG_ERROR() << "Unknown event in range sensor event file:" << eventType << code << value;
	}
}

int RangeSensorWorker::read()
{
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to read from uninitialized sensor, ignoring";
		return -1;
	}

	QReadLocker locker(&mDistanceLock);
	return mDistance;
}

int RangeSensorWorker::readRawData()
{
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to read from uninitialized sensor, ignoring";
		return -1;
	}

	return mRawDistance;
}
