/* Copyright 2013 Matvey Bryksin, Yurii Litvinov
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

#include "sensor3d.h"

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

Sensor3d::Sensor3d(int min, int max, const QString &controlFile)
	: mDeviceFileDescriptor(0)
	, mMax(max)
	, mMin(min)
{
	mReading << 0 << 0 << 0;

	mDeviceFileDescriptor = open(controlFile.toStdString().c_str(), O_SYNC, O_RDONLY);
	if (mDeviceFileDescriptor == -1) {
		qDebug() << "Cannot open input file";
		return;
	}

	mSocketNotifier = QSharedPointer<QSocketNotifier>(
			new QSocketNotifier(mDeviceFileDescriptor, QSocketNotifier::Read, this)
			);

	connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
	mSocketNotifier->setEnabled(true);
}

void Sensor3d::readFile()
{
	struct input_event event;

	if (::read(mDeviceFileDescriptor, reinterpret_cast<char *>(&event), sizeof(event)) != sizeof(event)) {
		qDebug() << "incomplete data read";
	} else {
		switch (event.type)
		{
		case EV_ABS:
			switch (event.code)
			{
			case ABS_X:
				mReading[0] = event.value;
				break;
			case ABS_Y:
				mReading[1] = event.value;
				break;
			case ABS_Z:
				mReading[2] = event.value;
				break;
			}
			break;
		case EV_SYN:
			return;
		}
	}
}

QVector<int> Sensor3d::read()
{
	return mReading;
}
