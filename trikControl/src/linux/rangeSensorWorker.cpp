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

#include <QtCore/QDebug>
#include <QtCore/QFileInfo>
#include <QtCore/QStringList>

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <linux/input.h>

#include <QsLog.h>

using namespace trikControl;

RangeSensorWorker::RangeSensorWorker(QString const &eventFile)
	: mEventFile(eventFile)
{
}

RangeSensorWorker::~RangeSensorWorker()
{
	stop();
}

void RangeSensorWorker::stop()
{
	if (mSocketNotifier) {
		disconnect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
		mSocketNotifier->setEnabled(false);
	}

	if (::close(mEventFileDescriptor) != 0) {
		QString const message = QString("%1: close failed: %2").arg(mEventFile.fileName()).arg(errno);
		QLOG_ERROR() << message;
		qDebug() << message;
	}

	mEventFileDescriptor = -1;
}

void RangeSensorWorker::init()
{
	QLOG_INFO() << "Opening" << mEventFile.fileName();
	qDebug() << "Opening" << mEventFile.fileName();

	/// @todo Is it possible to use QFile (and QFile::handle()) here?
	mEventFileDescriptor = open(mEventFile.fileName().toStdString().c_str(), O_SYNC | O_NONBLOCK, O_RDONLY);

	if (mEventFileDescriptor == -1) {
		QLOG_ERROR() << "Cannot open range sensor output file" << mEventFile.fileName();
		qDebug() << "Cannot open range sensor output file" << mEventFile.fileName();
		return;
	}

	mSocketNotifier.reset(new QSocketNotifier(mEventFileDescriptor, QSocketNotifier::Read));

	connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
	mSocketNotifier->setEnabled(true);
}

void RangeSensorWorker::readFile()
{
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
					mLock.lockForWrite();
					mDistance = event.value;
					mLock.unlock();
					break;
				case ABS_MISC:
					mLock.lockForWrite();
					mRawDistance = event.value;
					mLock.unlock();
					break;
				}
				break;
			case EV_SYN:
				mLock.lockForRead();
				emit newData(mDistance, mRawDistance);
				mLock.unlock();
				break;
		}
	}

	if (0 <= size && size < static_cast<int>(sizeof(event))) {
		QLOG_ERROR() << "incomplete data read";
		qDebug() << "incomplete data read";
	}

	mSocketNotifier->setEnabled(true);
}

int RangeSensorWorker::read()
{
	mLock.lockForRead();
	int const result = mDistance;
	mLock.unlock();
	return result;
}

int RangeSensorWorker::readRawData()
{
	mLock.lockForRead();
	int const result = mRawDistance;
	mLock.unlock();
	return result;
}
