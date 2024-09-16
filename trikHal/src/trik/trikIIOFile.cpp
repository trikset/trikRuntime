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

trikHal::trik::TrikIIOFile::TrikIIOFile(const QString &fileName, const QString &scanType)
	: mFileName(fileName)
{
	if (scanType == "be:s14/16>>2") {
		mScanType = ScanType::Accel;
	} else if (scanType == "le:s16/16>>0") {
		mScanType = ScanType::Gyro;
	} else {
		mScanType = ScanType::Undefined;
	}
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

		QVector<int> sensorValues;
		if (mScanType == ScanType::Accel) {
			sensorValues = {
				static_cast<int16_t>((buffer[0] << 8) | buffer[1]) >> 2,
				static_cast<int16_t>((buffer[2] << 8) | buffer[3]) >> 2,
				static_cast<int16_t>((buffer[4] << 8) | buffer[5]) >> 2,
			};
		} else if (mScanType == ScanType::Gyro) {
			sensorValues = {
				static_cast<int16_t>((buffer[1] << 8) | buffer[0]),
				static_cast<int16_t>((buffer[3] << 8) | buffer[2]),
				static_cast<int16_t>((buffer[5] << 8) | buffer[4]),
			};
		} else {
			size_t len = sizeof(buffer) / sizeof(buffer[0]);
			for (size_t i = 0; i < len; i++) {
				sensorValues.append(static_cast<int>(buffer[i]));
			}
			QLOG_ERROR() << "Unknown scan type for iio device";
		}

		emit newData(std::move(sensorValues), eventTime);
	}

	mSocketNotifier->setEnabled(true);
}
