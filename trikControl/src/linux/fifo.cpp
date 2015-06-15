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

#include "src/fifo.h"

#include <unistd.h>
#include <fcntl.h>
#include <linux/input.h>

#include <QtCore/QSocketNotifier>
#include <QtCore/QEventLoop>

#include <trikKernel/configurer.h>

#include <QsLog.h>

#include "src/configurerHelper.h"

using namespace trikControl;

Fifo::Fifo(const QString &virtualPort, const trikKernel::Configurer &configurer)
	: mFifoFileDescriptor(-1)
{
	const QString fileName = configurer.attributeByPort(virtualPort, "file");

	mFifoFileDescriptor = open(fileName.toStdString().c_str(), O_SYNC | O_NONBLOCK, O_RDONLY);
	if (mFifoFileDescriptor == -1) {
		QLOG_ERROR() << "Can't open FIFO file" << fileName;
		mState.fail();
		return;
	}

	mSocketNotifier.reset(new QSocketNotifier(mFifoFileDescriptor, QSocketNotifier::Read, this));

	connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
	mSocketNotifier->setEnabled(true);

	QLOG_INFO() << "Opened FIFO file" << fileName;
	mState.ready();
}

Fifo::~Fifo()
{
	close(mFifoFileDescriptor);
}

DeviceInterface::Status Fifo::status() const
{
	return mState.status();
}

QString Fifo::read()
{
	while (mCurrent.isEmpty()) {
		QEventLoop eventLoop;
		connect(this, SIGNAL(newData(QString)), &eventLoop, SLOT(quit()));
		eventLoop.exec();
	}

	const QString result = mCurrent;
	mCurrent = "";
	return result;
}

bool Fifo::hasData() const
{
	return mCurrent != "";
}

void Fifo::readFile()
{
	char data[4000] = {0};

	mSocketNotifier->setEnabled(false);

	if (::read(mFifoFileDescriptor, data, 4000) < 0) {
		QLOG_ERROR() << "FIFO read failed: " << strerror(errno);
		mState.fail();
		return;
	}

	mBuffer += data;

	if (mBuffer.contains("\n")) {
		QStringList lines = mBuffer.split('\n', QString::KeepEmptyParts);

		mBuffer = lines.last();
		lines.removeLast();
		mCurrent = lines.last();

		for (const QString line : lines) {
			emit newData(line);
		}
	}

	mSocketNotifier->setEnabled(true);
}
