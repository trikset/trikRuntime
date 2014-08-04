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

#include "src/sensor3dWorker.h"

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

using namespace trikControl;

Sensor3dWorker::Sensor3dWorker(int min, int max, const QString &controlFile)
	: mDeviceFileDescriptor(0)
	, mMax(max)
	, mMin(min)
{
	mReading << 0 << 0 << 0;

	mDeviceFileDescriptor = open(controlFile.toStdString().c_str(), O_SYNC | O_NONBLOCK, O_RDONLY);
	if (mDeviceFileDescriptor == -1) {
		qDebug() << "Cannot open input file " << controlFile;
		return;
	}

	mSocketNotifier.reset(
			new QSocketNotifier(mDeviceFileDescriptor, QSocketNotifier::Read, this)
			);

	connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
	mSocketNotifier->setEnabled(true);
}

void Sensor3dWorker::readFile()
{
	struct input_event event;
	int size = 0;

	mSocketNotifier->setEnabled(false);

	while ((size = ::read(mDeviceFileDescriptor, reinterpret_cast<char *>(&event), sizeof(event)))
			== static_cast<int>(sizeof(event)))
	{
		switch (event.type) {
			case EV_ABS:
				switch (event.code) {
				case ABS_X:
					mLock.lockForWrite();
					mReading[0] = event.value;
					mLock.unlock();
					break;
				case ABS_Y:
					mLock.lockForWrite();
					mReading[1] = event.value;
					mLock.unlock();
					break;
				case ABS_Z:
					mLock.lockForWrite();
					mReading[2] = event.value;
					mLock.unlock();
					break;
				}
				break;
			case EV_SYN:
				break;
		}
	}

	if (0 <= size && size < static_cast<int>(sizeof(event))) {
		qDebug() << "incomplete data read";
	}

	mSocketNotifier->setEnabled(true);
}

QVector<int> Sensor3dWorker::read()
{
	mLock.lockForRead();
	QVector<int> const result = mReading;
	mLock.unlock();
	return result;
}
