#include "trikIIOFile.h"

#include <fcntl.h>
#include <unistd.h>

#include <QsLog.h>

#include <trikKernel/timeVal.h>


trikHal::trik::TrikIIOFile::TrikIIOFile(const QString &fileName)
	: mFileName(fileName)
{
}

bool trikHal::trik::TrikIIOFile::open()
{
	QLOG_INFO() << "Openning" << mFileName;
	mIIOFileDescriptor = ::open(mFileName.toStdString().c_str(), O_NONBLOCK | O_RDONLY);
	if (mIIOFileDescriptor == -1) {
		QLOG_ERROR() << QString("%1: open failed: %2").arg(mFileName).arg(strerror(errno));
		return false;
	}

	mSocketNotifier.reset(new QSocketNotifier(mIIOFileDescriptor, QSocketNotifier::Read));

	connect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
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
		disconnect(mSocketNotifier.data(), SIGNAL(activated(int)), this, SLOT(readFile()));
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
