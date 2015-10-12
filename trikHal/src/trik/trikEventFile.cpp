/* Copyright 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include "trikEventFile.h"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>

#include <QtCore/QFileInfo>
#include <QtCore/QStringList>
#include <QtCore/QTimer>
#include <QtCore/QEventLoop>
#include <QtCore/QSocketNotifier>

#include <QsLog.h>

using namespace trikHal::trik;

TrikEventFile::TrikEventFile(const QString &fileName)
	: mFileName(fileName)
{
}

TrikEventFile::~TrikEventFile()
{
}

bool TrikEventFile::open()
{
	QLOG_INFO() << "Opening" << mFileName;

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
		QLOG_ERROR() << "Cannot open event file" << mFileName;
		return false;
	}

	mSocketNotifier.reset(new QSocketNotifier(mEventFileDescriptor, QSocketNotifier::Read));

	connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
	mSocketNotifier->setEnabled(true);
	return true;
}

void TrikEventFile::tryOpenEventFile()
{
	if (mEventFileDescriptor != -1) {
		return;
	}

	mEventFileDescriptor = ::open(mFileName.toStdString().c_str(), O_SYNC | O_NONBLOCK, O_RDONLY);

	if (mEventFileDescriptor != -1 && !mInitWaitingLoop.isNull() && mInitWaitingLoop->isRunning()) {
		mInitWaitingLoop->quit();
	}
}

bool TrikEventFile::close()
{
	if (mEventFileDescriptor == -1) {
		return false;
	}

	if (mSocketNotifier) {
		disconnect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
		mSocketNotifier->setEnabled(false);
	}

	if (::close(mEventFileDescriptor) != 0) {
		QLOG_ERROR() << QString("%1: close failed: %2").arg(mFileName).arg(strerror(errno));
		return false;
	}

	mEventFileDescriptor = -1;
	return true;
}

void TrikEventFile::cancelWaiting()
{
	mInitWaitingLoop->quit();
}

QString TrikEventFile::fileName() const
{
	return mFileName;
}

void TrikEventFile::readFile()
{
	struct input_event event;
	int size = 0;

	mSocketNotifier->setEnabled(false);

	while ((size = ::read(mEventFileDescriptor, reinterpret_cast<char *>(&event), sizeof(event)))
			== static_cast<int>(sizeof(event)))
	{
		EventType eventType = EventType::unknown;
		switch (event.type) {
			case EV_ABS:
				switch (event.code) {
				case ABS_DISTANCE:
					eventType = EventType::evAbsDistance;
					break;
				case ABS_MISC:
					eventType = EventType::evAbsMisc;
					break;
				case ABS_X:
					eventType = EventType::evAbsX;
					break;
				case ABS_Y:
					eventType = EventType::evAbsY;
					break;
				case ABS_Z:
					eventType = EventType::evAbsZ;
					break;
				}

				break;
			case EV_KEY:
				eventType = EventType::evKey;
				break;
			case EV_SYN:
				eventType = EventType::evSyn;
				break;
		}

		emit newEvent(eventType, event.code, event.value);
	}

	if (0 <= size && size < static_cast<int>(sizeof(event))) {
		QLOG_ERROR() << "incomplete data read from" << mFileName;
	}

	mSocketNotifier->setEnabled(true);
}

