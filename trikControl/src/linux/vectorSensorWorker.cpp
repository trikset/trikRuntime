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

#include <QtCore/QDebug>

#include <cmath>
#include <unistd.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <errno.h>
#include <linux/ioctl.h>
#include <linux/input.h>

#include <QsLog.h>

using namespace trikControl;

VectorSensorWorker::VectorSensorWorker(const QString &controlFile, DeviceState &state)
	: mDeviceFileDescriptor(0)
	, mState(state)
{
	mReading << 0 << 0 << 0;
	mReadingUnsynced = mReading;

	mDeviceFileDescriptor = open(controlFile.toStdString().c_str(), O_SYNC | O_NONBLOCK, O_RDONLY);
	if (mDeviceFileDescriptor == -1) {
		QLOG_ERROR() << "Cannot open input file " << controlFile;
		mState.fail();
		return;
	}

	mSocketNotifier.reset(new QSocketNotifier(mDeviceFileDescriptor, QSocketNotifier::Read, this));

	connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
	mSocketNotifier->setEnabled(true);
}

void VectorSensorWorker::readFile()
{
	struct input_event event;
	int size = 0;

	while ((size = ::read(mDeviceFileDescriptor, reinterpret_cast<char *>(&event), sizeof(event)))
			== static_cast<int>(sizeof(event)))
	{
		switch (event.type) {
			case EV_ABS:
				switch (event.code) {
				case ABS_X:
					mReadingUnsynced[0] = event.value;
					break;
				case ABS_Y:
					mReadingUnsynced[1] = event.value;
					break;
				case ABS_Z:
					mReadingUnsynced[2] = event.value;
					break;
				}
				break;
			case EV_SYN:
				mReading.swap(mReadingUnsynced);
				emit newData(mReading);

				break;
		}
	}

	if (0 <= size && size < static_cast<int>(sizeof(event))) {
		QLOG_ERROR() << "incomplete data read";
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
