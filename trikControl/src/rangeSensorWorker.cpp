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

#include <QtCore/QTextStream>
#include <QtCore/QThread>
#include <QsLog.h>

#include <trikHal/hardwareAbstractionInterface.h>
#include <trikHal/inputDeviceFileInterface.h>

#include <utility>

using namespace trikControl;

RangeSensorWorker::RangeSensorWorker(const trikHal::HardwareAbstractionInterface &hardwareAbstraction,
	QString sensorFile, DeviceState &state, int minValue, int maxValue, QString filterName, int samplingPeriodMs)
	: mSamplingPeriodMs(samplingPeriodMs)
	, mHardwareAbstraction(hardwareAbstraction)
	, mState(state)
	, mSensorFile(std::move(sensorFile))
	, mMinValue(minValue)
	, mMaxValue(maxValue)
	, mFilterName(std::move(filterName))
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
		if (mReadTimer) {
			mReadTimer->stop();
		}

		if (mDistanceFile) {
			mDistanceFile->close();
		}

		mState.off();
	} else if (mState.status() == DeviceInterface::Status::starting) {
		// Sensor was not yet started, abort waiting.
		if (mReadTimer) {
			mReadTimer->stop();
		}

		if (mDistanceFile) {
			mDistanceFile->close();
		}

		mState.ready();
		mState.off();
	} else {
		QLOG_ERROR() << "Trying to stop range sensor that is not started, ignoring";
	}
}

void RangeSensorWorker::init()
{
	mDistanceFile.reset(mHardwareAbstraction.createInputDeviceFile(mSensorFile));

	if (!mDistanceFile->open()) {
		if (mState.status() == DeviceInterface::Status::off) {
			// Launching was aborted, do nothing.
			return;
		}

		QLOG_ERROR() << "Failed to open IIO sensor file" << mSensorFile;
		mState.fail();
		return;
	}

	if (mFilterName == "median3") {
		mDataFilter.reset(new DataFilter(mMinValue, mMaxValue, "median3"));
	}

	mElapsedTimer.start();

	mReadTimer.reset(new QTimer(this));
	mReadTimer->setInterval(mSamplingPeriodMs);
	connect(mReadTimer.data(), &QTimer::timeout, this, &RangeSensorWorker::onTimer);
	mReadTimer->start();

	mState.ready();
}

void RangeSensorWorker::onTimer()
{
	if (!mState.isReady()) {
		return;
	}

	mDistanceFile->reset();
	auto &stream = mDistanceFile->stream();
	stream.resetStatus();

	auto value = 0;
	stream >> value;
	if (stream.status() != QTextStream::Ok) {
		QLOG_ERROR() << "Failed to parse IIO sensor value from" << mSensorFile;
		return;
	}

	const auto elapsedMs = mElapsedTimer.elapsed();
	const trikKernel::TimeVal eventTime(static_cast<int>(elapsedMs / 1000),
		static_cast<int>((elapsedMs % 1000) * 1000));

	QWriteLocker locker(&mDistanceLock);
	mRawDistance = value;
	mDistance = !mDataFilter ? value : mDataFilter->applyFilter(value);
	locker.unlock();

	Q_EMIT newData(mDistance, mRawDistance, eventTime);
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

	QReadLocker locker(&mDistanceLock);
	return mRawDistance;
}
