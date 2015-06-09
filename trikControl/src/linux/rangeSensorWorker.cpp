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

#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtCore/QEventLoop>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>

#include <QsLog.h>

using namespace trikControl;

RangeSensorWorker::RangeSensorWorker(const QString &eventFile, DeviceState &state)
	: mEventFile(eventFile)
	, mState(state)
{
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
		if (mSocketNotifier) {
			disconnect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
			mSocketNotifier->setEnabled(false);
		}

		if (::close(mEventFileDescriptor) != 0) {
			QLOG_ERROR() << QString("%1: close failed: %2").arg(mEventFile.fileName()).arg(strerror(errno));
			mState.fail();
		}

		mEventFileDescriptor = -1;
		mState.off();
	} else {
		QLOG_ERROR() << "Trying to stop range sensor that is not started, ignoring";
	}
}

void RangeSensorWorker::init()
{
	mState.start();

	QLOG_INFO() << "Opening" << mEventFile.fileName();

	tryOpenEventFile();
	if (mEventFileDescriptor == -1) {
		// Give driver some time to create event file.
		mInitWaitingLoop.reset(new QEventLoop());
		QTimer checkTimer;
		QObject::connect(&checkTimer, SIGNAL(timeout()), this, SLOT(tryOpenEventFile()));
		checkTimer.start(100);

		QTimer::singleShot(2000, mInitWaitingLoop.data(), SLOT(quit()));

		mInitWaitingLoop->exec();
	}

	if (mEventFileDescriptor == -1) {
		QLOG_ERROR() << "Cannot open range sensor output file" << mEventFile.fileName();
		mState.fail();
		return;
	}

	mSocketNotifier.reset(new QSocketNotifier(mEventFileDescriptor, QSocketNotifier::Read));

	connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
	mSocketNotifier->setEnabled(true);
	mState.ready();
}

void RangeSensorWorker::readFile()
{
	if (!mState.isReady()) {
		return;
	}

	struct input_event event;
	int size = 0;

	mSocketNotifier->setEnabled(false);

	while ((size = ::read(mEventFileDescriptor, reinterpret_cast<char *>(&event), sizeof(event)))
			== static_cast<int>(sizeof(event)))
	{
		switch (event.type) {
			case EV_ABS:
				switch (event.code) {
				case ABS_DISTANCE:
					mDistance = event.value;
					break;
				case ABS_MISC:
					mRawDistance = event.value;
					break;
				}
				break;
			case EV_SYN:
				emit newData(mDistance, mRawDistance);
				break;
		}
	}

	if (0 <= size && size < static_cast<int>(sizeof(event))) {
		QLOG_ERROR() << "incomplete data read";
	}

	mSocketNotifier->setEnabled(true);
}

void RangeSensorWorker::tryOpenEventFile()
{
	mEventFileDescriptor = open(mEventFile.fileName().toStdString().c_str(), O_SYNC | O_NONBLOCK, O_RDONLY);

	if (mEventFileDescriptor != -1 && !mInitWaitingLoop.isNull() && mInitWaitingLoop->isRunning()) {
		mInitWaitingLoop->quit();
	}
}

int RangeSensorWorker::read()
{
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to read from uninitialized sensor, ignoring";
		return 0;
	}

	return mDistance;
}

int RangeSensorWorker::readRawData()
{
	if (!mState.isReady()) {
		QLOG_ERROR() << "Trying to read from uninitialized sensor, ignoring";
		return 0;
	}

	return mRawDistance;
}
