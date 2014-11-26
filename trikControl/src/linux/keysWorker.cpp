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

#include "src/keysWorker.h"

#include <QtCore/QDebug>
#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#include "QsLog.h"

using namespace trikControl;

KeysWorker::KeysWorker(QString const &keysPath)
{
	mKeysFileDescriptor = open(keysPath.toStdString().c_str(), O_SYNC, O_RDONLY);
	if (mKeysFileDescriptor == -1) {
		QLOG_ERROR() << "cannot open keys input file" << keysPath;
		qDebug() << "cannot open keys input file" << keysPath;
		return;
	}

	mSocketNotifier.reset(new QSocketNotifier(mKeysFileDescriptor, QSocketNotifier::Read, this));

	connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readKeysEvent()));
	mSocketNotifier->setEnabled(true);
}

void KeysWorker::reset()
{
	mLock.lockForWrite();
	mWasPressed.clear();
	mLock.unlock();
}

bool KeysWorker::wasPressed(int code)
{
	mLock.lockForRead();
	bool result = mWasPressed.contains(code);
	mLock.unlock();

	if (result) {
		mLock.lockForWrite();
		mWasPressed.remove(code);
		mLock.unlock();
	}

	return result;
}

void KeysWorker::readKeysEvent()
{
	struct input_event event;

	if (read(mKeysFileDescriptor, reinterpret_cast<char*>(&event), sizeof(event)) != sizeof(event)) {
		QLOG_ERROR() << "keys: incomplete data read";
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
		if (mButtonValue) {
			mLock.lockForWrite();
			mWasPressed.insert(mButtonCode);
			mLock.unlock();
		}

		emit buttonPressed(mButtonCode, mButtonValue);
		break;
	}
}
