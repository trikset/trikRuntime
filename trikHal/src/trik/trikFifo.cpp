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

#include "trikFifo.h"

#include <unistd.h>
#include <fcntl.h>
#include <errno.h>

#include <QtCore/QSocketNotifier>
#include <QtCore/QStringList>

#include <QsLog.h>

using namespace trikHal::trik;

TrikFifo::TrikFifo(const QString &fileName)
	: mFileName(fileName)
	, mFileDescriptor(-1)
{
}

TrikFifo::~TrikFifo()
{
	close();
}

bool TrikFifo::open()
{

	qDebug() << "open 0 file desc" << mFileDescriptor;


	mFileDescriptor = ::open(mFileName.toStdString().c_str(), O_SYNC | O_NONBLOCK | O_RDWR | O_NDELAY);



	qDebug() << "open 1 file desc" << mFileDescriptor;

	if (mFileDescriptor == -1) {
		QLOG_ERROR() << "Can't open FIFO file" << mFileName;
		return false;
	}

	//mSocketNotifier.reset(new QSocketNotifier(mFileDescriptor, QSocketNotifier::Read, this));
	mSocketNotifier.reset(new QSocketNotifier(mFileDescriptor, QSocketNotifier::Read));

	connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
	mSocketNotifier->setEnabled(true);

	qDebug() << "open 2 file desc" << mFileDescriptor;

	QLOG_INFO() << "Opened FIFO file" << mFileName;
	return true;
}

void TrikFifo::readFile()
{
	char data[4000] = {0};
	static int counter;

	counter++;

	mSocketNotifier->setEnabled(false);

	qDebug() << "read 0 file desc" << mFileDescriptor;
	qDebug() << data;
	qDebug() << counter;


	if (::read(mFileDescriptor, &data, 4000) < 0) {
		qDebug() << "read 1 file desc" << mFileDescriptor;
		qDebug() << data;
		qDebug() << counter;


		QLOG_ERROR() << "FIFO read failed: " << strerror(errno);
		emit readError();
		return;
	}

	qDebug() << "read 2 file desc" << mFileDescriptor;
	qDebug() << data;
	qDebug() << counter;


	mBuffer += data;

	if (mBuffer.contains("\n")) {
		qDebug() << "read 3 file desc" << mFileDescriptor;
		qDebug() << data;

		QStringList lines = mBuffer.split('\n', QString::KeepEmptyParts);

		mBuffer = lines.last();
		lines.removeLast();

		for (const QString &line : lines) {
			emit newData(line);
		}
	}

	mSocketNotifier->setEnabled(true);
}

bool TrikFifo::close()
{
	if (mFileDescriptor != -1) {
		qDebug() << "close file desc" << mFileDescriptor;
		bool result = ::close(mFileDescriptor) == 0;
		mFileDescriptor = -1;
		return result;
	}

	return false;
}

QString TrikFifo::fileName()
{
	return mFileName;
}
