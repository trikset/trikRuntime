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
#include <termios.h>

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
	mFileDescriptor = ::open(mFileName.toStdString().c_str(), O_NOCTTY | O_RDWR | O_NONBLOCK | O_CLOEXEC);

	if (mFileDescriptor == -1) {
		QLOG_ERROR() << "Failed to open FIFO file in read-write mode" << mFileName;
		mFileDescriptor = ::open(mFileName.toStdString().c_str(), O_NOCTTY | O_RDONLY | O_NONBLOCK | O_CLOEXEC);
		if (mFileDescriptor == -1) {
			QLOG_ERROR() << "Failed to open FIFO file in read-only mode" << mFileName;
			return false;
		}
	}

	if (isatty(mFileDescriptor)) {
		termios t {};
		QLOG_INFO() << "Using tty as FIFO:" << mFileName;
		if (tcgetattr(mFileDescriptor, &t)) {
			QLOG_ERROR() << __PRETTY_FUNCTION__ << ": tcgetattr failed for" << mFileName;
		} else {
			t.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
			t.c_oflag &= ~(OPOST);
			t.c_cflag |= CS8;
			t.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
			t.c_cc[VMIN] = 1;
			t.c_cc[VTIME] = 0;
			termios t1 = t;
			tcsetattr(mFileDescriptor, TCSANOW, &t);
			if (tcgetattr(mFileDescriptor, &t1)
					|| std::make_tuple(t.c_iflag, t.c_oflag, t.c_cflag, t.c_lflag)
						!= std::make_tuple(t1.c_iflag, t1.c_oflag, t1.c_cflag, t1.c_lflag)) {
				QLOG_ERROR() << __PRETTY_FUNCTION__ << ": tcsetattr failed for" << mFileName;
			}
			if (cfsetospeed (&t, B230400)) {
				QLOG_ERROR() << __PRETTY_FUNCTION__ << ": cfsetospeed 230400 failed for" << mFileName;
			}
			if (cfsetispeed (&t, B230400)) {
				QLOG_ERROR() << __PRETTY_FUNCTION__ << ": cfsetispeed 230400 failed for" << mFileName;
			}
		}
	}

	mSocketNotifier.reset(new QSocketNotifier(mFileDescriptor, QSocketNotifier::Read));

	connect(mSocketNotifier.data(), &QSocketNotifier::activated, this, &TrikFifo::readFile);
	mSocketNotifier->setEnabled(true);

	QLOG_INFO() << "Opened FIFO file" << mFileName;
	return true;
}

void TrikFifo::readFile()
{
	QVector<uint8_t> bytes(4000);
	mSocketNotifier->setEnabled(false);
	auto bytesRead = ::read(mFileDescriptor, bytes.data(), static_cast<size_t>(bytes.size()));
	if (bytesRead < 0) {
		if (errno != EAGAIN) {
			QLOG_ERROR() << "FIFO read failed: " << strerror(errno) << "in" << mFileName;
			emit readError();
		}
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
