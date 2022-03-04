/* Copyright 2021 CyberTech Labs Ltd.
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

#include "trikIIOFile.h"

#include <fcntl.h>
#include <unistd.h>

#include <QsLog.h>

#include <trikKernel/timeVal.h>


trikHal::trik::TrikIIOFile::TrikIIOFile(const QString &fileName)
	: mFileName(fileName)
{
}

trikHal::trik::TrikIIOFile::~TrikIIOFile()
{
	close();
}

bool trikHal::trik::TrikIIOFile::open()
{
	QLOG_INFO() << "Openning" << mFileName;
	mIIOFileDescriptor = ::open(mFileName.toStdString().c_str(), O_NONBLOCK | O_RDONLY | O_CLOEXEC);
	if (mIIOFileDescriptor == -1) {
		QLOG_ERROR() << QString("%1: open failed: %2").arg(mFileName).arg(strerror(errno));
		return false;
	}

	mSocketNotifier.reset(new QSocketNotifier(mIIOFileDescriptor, QSocketNotifier::Read));

	connect(mSocketNotifier.data(), &QSocketNotifier::activated, this, &TrikIIOFile::readFile);
	mSocketNotifier->setEnabled(true);

	return true;
}

bool trikHal::trik::TrikIIOFile::close()
{
	QLOG_INFO() << "Closing" << mFileName;
	if (mIIOFileDescriptor == -1) {
		return false;
	}

	if (mSocketNotifier) {
		disconnect(mSocketNotifier.data(), &QSocketNotifier::activated, this, &TrikIIOFile::readFile);
		mSocketNotifier->setEnabled(false);
	}

	if (::close(mIIOFileDescriptor) != 0) {
		QLOG_ERROR() << QString("%1: close failed: %2").arg(mFileName).arg(strerror(errno));
		return false;
	}

	mIIOFileDescriptor = -1;
	return true;
}

QString trikHal::trik::TrikIIOFile::fileName() const
{
	return mFileName;
}

bool trikHal::trik::TrikIIOFile::isOpened() const
{
	return mIIOFileDescriptor != -1;
}

void trikHal::trik::TrikIIOFile::readFile()
{
	mSocketNotifier->setEnabled(false);

	uint8_t buffer[16];
	if (::read(mIIOFileDescriptor, &buffer, sizeof(buffer)) == -1) {
		QLOG_ERROR() << QString("%1: read failed: %2").arg(mFileName).arg(strerror(errno));
	} else {
		uint64_t timestamp;
		memcpy(&timestamp, &buffer[8], sizeof(timestamp));
		const uint64_t timestamp_mcsec = timestamp / 1000;
		trikKernel::TimeVal eventTime(timestamp_mcsec / 1000000, timestamp_mcsec % 1000000);

		// TODO: add comments
		auto convert_axis = [](uint16_t b_byte, uint16_t s_byte) {
			return static_cast<int16_t>((b_byte << 8) | s_byte) >> 2;
		};

		QVector<int> sensorValues = {
			convert_axis(buffer[0], buffer[1]),
			convert_axis(buffer[2], buffer[3]),
			convert_axis(buffer[4], buffer[5]) };

		emit newData(sensorValues, eventTime);
	}

	mSocketNotifier->setEnabled(true);
}
