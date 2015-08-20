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

TrikFifo::TrikFifo()
	: mFifoFileDescriptor(-1)
{
}

TrikFifo::~TrikFifo()
{
	close();
}

bool TrikFifo::open(const QString &fileName)
{
	mFifoFileDescriptor = ::open(fileName.toStdString().c_str(), O_SYNC | O_NONBLOCK, O_RDONLY);
	if (mFifoFileDescriptor == -1) {
		QLOG_ERROR() << "Can't open FIFO file" << fileName;
		return false;
	}

	mSocketNotifier.reset(new QSocketNotifier(mFifoFileDescriptor, QSocketNotifier::Read, this));

	connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
	mSocketNotifier->setEnabled(true);

	QLOG_INFO() << "Opened FIFO file" << fileName;
	return true;
}

void TrikFifo::readFile()
{
	char data[4000] = {0};

	mSocketNotifier->setEnabled(false);

	if (::read(mFifoFileDescriptor, data, 4000) < 0) {
		QLOG_ERROR() << "FIFO read failed: " << strerror(errno);
		emit readError();
		return;
	}

	mBuffer += data;

	if (mBuffer.contains("\n")) {
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
	if (mFifoFileDescriptor != -1) {
		return ::close(mFifoFileDescriptor) == 0;
	}

	return false;
}
