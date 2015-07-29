/* Copyright 2015 CyberTech Labs Ltd.
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

#include <QtCore/QProcess>
#include <QtCore/QDebug>
#include <QtCore/QSocketNotifier>

#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

using namespace trikRuntimeKiller;

Keys::Keys()
{
	mKeysFileDescriptor = open("/dev/input/by-path/platform-gpio-keys-event", O_SYNC, O_RDONLY);
	if (mKeysFileDescriptor == -1) {
		qDebug() << "cannot open keys input file";
		return;
	}

	mSocketNotifier.reset(new QSocketNotifier(mKeysFileDescriptor, QSocketNotifier::Read, this));

	connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readKeysEvent()));
	mSocketNotifier->setEnabled(true);
}

Keys::~Keys()
{
}

void Keys::readKeysEvent()
{
	struct input_event event;

	if (read(mKeysFileDescriptor, reinterpret_cast<char*>(&event), sizeof(event)) != sizeof(event)) {
		return;
	}

	switch (event.type)
	{
	case EV_KEY: {
		mButtonCode = static_cast<int>(event.code);
		mButtonValue = static_cast<int>(event.value);
		break;
	}
	case EV_SYN: {
		switch (mButtonCode) {
		case 108: {
			mDownPressed = mButtonValue == 1;
			break;
		}
		case 28: {
			mEnterPressed = mButtonValue == 1;
			break;
		}
		case 1: {
			mEscapePressed = mButtonValue == 1;
			break;
		}
		default: {
			break;
		}
		}

		if (mEnterPressed && mDownPressed && mEscapePressed) {
			QProcess::startDetached("/usr/bin/killall", {"trikGui"});
		}
	}
	}
}
