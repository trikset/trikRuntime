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

#include "keys.h"

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

Keys::Keys(const QString &keysPath)
	: mButtonCode(KEY_RESERVED)
	, mButtonValue(0)
{
	mKeysFileDescriptor = open(keysPath.toAscii().data(), O_SYNC, O_RDONLY);
	if (mKeysFileDescriptor == -1) {
		qDebug() << "cannot open keys input file";
		return;
	}

	mSocketNotifier = QSharedPointer<QSocketNotifier>
			(new QSocketNotifier(mKeysFileDescriptor, QSocketNotifier::Read, this));

	connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readKeysEvent()));
	mSocketNotifier->setEnabled(true);
}

void Keys::readKeysEvent()
{
	struct input_event event;

	if (read(mKeysFileDescriptor, reinterpret_cast<char*>(&event), sizeof(event)) != sizeof(event)) {
		qDebug() << "keys: incomplete data read";
		return;
	}

	switch (event.type)
	{
	case EV_KEY:
		mButtonCode = static_cast<int>(event.code);
		mButtonValue = static_cast<int>(event.value);
		break;
	case EV_SYN:
		emit buttonPressed(mButtonCode, mButtonValue);
		break;
	}
}
