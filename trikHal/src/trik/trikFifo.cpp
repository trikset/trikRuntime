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
#include <cerrno>

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
	mFileDescriptor = ::open(mFileName.toStdString().c_str(), O_RDONLY | O_NONBLOCK);

	if (mFileDescriptor == -1) {
		QLOG_ERROR() << "Can't open FIFO file" << mFileName;
		return false;
	}

	mSocketNotifier.reset(new QSocketNotifier(mFileDescriptor, QSocketNotifier::Read));

	connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
	mSocketNotifier->setEnabled(true);

	QLOG_INFO() << "Opened FIFO file" << mFileName;
	return true;
}

void TrikFifo::readFile()
{
	QVector<uint8_t> bytes(4000);
//	QByteArray bytes(4000, ' '); //Andrei
	mSocketNotifier->setEnabled(false);
	auto bytesRead = ::read(mFileDescriptor, bytes.data(), static_cast<size_t>(bytes.size()));
	if (bytesRead < 0) {
		QLOG_ERROR() << "FIFO read failed: " << strerror(errno) << "in" << mFileName;
		emit readError();
		mSocketNotifier->setEnabled(true);
		return;
	}
	bytes.resize(bytesRead);
	emit newData(bytes);
	mBuffer += QByteArray(reinterpret_cast<char*>(bytes.data()), bytes.size());
	if (mBuffer.contains("\n")) {
		QStringList lines = mBuffer.split('\n', QString::KeepEmptyParts);

		mBuffer = lines.last();
		lines.removeLast();

		for (auto &&line : lines) {
			emit newLine(line);
		}
	}

	mSocketNotifier->setEnabled(true);
}

bool TrikFifo::close()
{
	if (mFileDescriptor != -1) {
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
