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

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <utility>

#include <fcntl.h>
#include <unistd.h>

#include <QDateTime>
#include <QDir>
#include <QFileInfo>
#include <QStringList>
#include <QtGlobal>

#include <trikKernel/fileUtils.h>
#include <trikKernel/timeVal.h>

#include <QsLog.h>

namespace {

constexpr int readRecordsPerChunk = 16;
constexpr int timestampBytes = 8;

bool readSysfsValue(const QString &path, QString *value)
{
	if (!QFileInfo::exists(path)) {
		return false;
	}

	try {
		*value = trikKernel::FileUtils::readFromFile(path).trimmed();
		return true;
	} catch (const trikKernel::TrikRuntimeException &) {
		return false;
	}
}

/// Parses a scan element type string like `le:s16/16>>0` or `be:s14/16>>2`.
bool parseScanType(const QString &type, trikHal::IIOChannelFormat *format)
{
	const auto colonPos = type.indexOf(QLatin1Char(':'));
	const auto slashPos = type.indexOf(QLatin1Char('/'));
	const auto shiftPos = type.indexOf(QStringLiteral(">>"));
	if (colonPos < 0 || slashPos < 0 || shiftPos < 0 || slashPos <= colonPos || shiftPos <= slashPos) {
		return false;
	}

	const auto signAndBits = type.mid(colonPos + 1, slashPos - colonPos - 1);
	if (signAndBits.isEmpty()) {
		return false;
	}

	const auto sign = signAndBits.at(0);
	if (sign != QLatin1Char('s') && sign != QLatin1Char('u')) {
		return false;
	}

	auto storageOk = false;
	auto shiftOk = false;
	const auto storageBits = type.mid(slashPos + 1, shiftPos - slashPos - 1).toInt(&storageOk);
	const auto shift = type.mid(shiftPos + 2).toInt(&shiftOk);
	if (!storageOk || !shiftOk || storageBits <= 0 || storageBits % 8 != 0 || shift < 0) {
		return false;
	}

	format->storageBits = storageBits;
	format->bigEndian = type.startsWith(QStringLiteral("be:"));
	format->isSigned = sign == QLatin1Char('s');
	format->shift = shift;
	return true;
}

/// Maps a sysfs channel name (`in_magn_x`, `in_temp`, ...) to a channel id.
bool parseChannelName(const QString &name, trikHal::IIOChannelId *id)
{
	if (name == QLatin1String("in_temp")) {
		id->type = trikHal::IIOChanType::Temp;
		id->axis = trikHal::IIOAxis::None;
		return true;
	}

	if (name == QLatin1String("in_timestamp")) {
		id->type = trikHal::IIOChanType::Timestamp;
		id->axis = trikHal::IIOAxis::None;
		return true;
	}

	const auto parts = name.split(QLatin1Char('_'));
	if (parts.size() != 3 || parts.at(0) != QLatin1String("in")) {
		return false;
	}

	const auto &type = parts.at(1);
	const auto &axis = parts.at(2);

	if (axis == QLatin1String("x")) {
		id->axis = trikHal::IIOAxis::X;
	} else if (axis == QLatin1String("y")) {
		id->axis = trikHal::IIOAxis::Y;
	} else if (axis == QLatin1String("z")) {
		id->axis = trikHal::IIOAxis::Z;
	} else {
		return false;
	}

	if (type == QLatin1String("accel")) {
		id->type = trikHal::IIOChanType::Accel;
	} else if (type == QLatin1String("anglvel")) {
		id->type = trikHal::IIOChanType::AnglVel;
	} else if (type == QLatin1String("magn")) {
		id->type = trikHal::IIOChanType::Magn;
	} else {
		return false;
	}

	return true;
}

quint64 readRaw(const quint8 *data, const trikHal::IIOChannelFormat &format)
{
	const auto bytes = format.storageBits / 8;
	quint64 raw = 0;
	if (format.bigEndian) {
		for (int i = 0; i < bytes; ++i) {
			raw = (raw << 8) | data[i];
		}
	} else {
		for (int i = bytes - 1; i >= 0; --i) {
			raw = (raw << 8) | data[i];
		}
	}

	return raw;
}

/// Decodes a channel value (up to 32 bits) honoring sign, endianness and shift.
int decodeValue(const quint8 *data, const trikHal::IIOChannelFormat &format)
{
	const auto bytes = format.storageBits / 8;
	if (bytes <= 0 || bytes > 4) {
		return 0;
	}

	const auto raw = static_cast<quint32>(readRaw(data, format));
	const auto bits = format.storageBits;

	if (!format.isSigned) {
		return static_cast<int>(raw >> format.shift);
	}

	qint64 signedRaw = raw;
	if (bits < 32) {
		const auto signBit = quint32(1) << (bits - 1);
		if (raw & signBit) {
			signedRaw = static_cast<qint64>(raw) - (static_cast<qint64>(1) << bits);
		}
	} else if (bits == 32) {
		signedRaw = static_cast<qint32>(raw);
	}

	return static_cast<int>(signedRaw >> format.shift);
}

qint64 decodeTimestamp(const quint8 *data, const trikHal::IIOChannelFormat &format)
{
	if (format.storageBits != timestampBytes * 8) {
		return 0;
	}

	return static_cast<qint64>(readRaw(data, format));
}

trikKernel::TimeVal timeValFromNs(qint64 timestampNs)
{
	const auto timestampMcsec = static_cast<quint64>(timestampNs) / 1000;
	return {static_cast<int>(timestampMcsec / 1000000), static_cast<int>(timestampMcsec % 1000000)};
}

trikKernel::TimeVal currentTime()
{
	const auto nowMs = QDateTime::currentMSecsSinceEpoch();
	return {static_cast<int>(nowMs / 1000), static_cast<int>((nowMs % 1000) * 1000)};
}

}

using namespace trikHal::trik;

TrikIIOFile::TrikIIOFile(QString fileName)
	: mFileName(std::move(fileName))
{
}

TrikIIOFile::~TrikIIOFile()
{
	close();
}

bool TrikIIOFile::buildLayout(QString *error)
{
	const auto deviceName = QFileInfo(mFileName).fileName();
	const auto scanDir = QStringLiteral("/sys/bus/iio/devices/") + deviceName + QStringLiteral("/scan_elements");

	QDir dir(scanDir);
	if (!dir.exists()) {
		*error = QStringLiteral("scan_elements directory not found: ") + scanDir;
		return false;
	}

	mChannels.clear();
	mValueChannels.clear();
	mTimestampChannel = -1;
	mRecordBytes = 0;

	const auto entries = dir.entryList(QStringList {QStringLiteral("in_*_en")}, QDir::Files);
	for (const auto &entry : entries) {
		auto enabled = QString();
		if (!readSysfsValue(scanDir + QLatin1Char('/') + entry, &enabled) || enabled != QLatin1String("1")) {
			continue;
		}

		auto name = entry;
		name.chop(3); // strip "_en"

		auto channel = ScanChannel {};
		channel.name = name;
		if (!parseChannelName(name, &channel.id)) {
			channel.id.type = trikHal::IIOChanType::Unknown;
			channel.id.axis = trikHal::IIOAxis::None;
		}
		channel.isTimestamp = channel.id.type == trikHal::IIOChanType::Timestamp;

		auto indexText = QString();
		if (!readSysfsValue(scanDir + QLatin1Char('/') + name + QStringLiteral("_index"), &indexText)) {
			*error = QStringLiteral("can not read scan index for ") + name;
			return false;
		}

		auto indexOk = false;
		channel.index = indexText.toInt(&indexOk);
		if (!indexOk) {
			*error = QStringLiteral("invalid scan index for ") + name;
			return false;
		}

		auto typeText = QString();
		if (!readSysfsValue(scanDir + QLatin1Char('/') + name + QStringLiteral("_type"), &typeText)) {
			*error = QStringLiteral("can not read scan type for ") + name;
			return false;
		}

		if (!parseScanType(typeText, &channel.format)) {
			*error = QStringLiteral("unsupported scan type '%1' for %2").arg(typeText, name);
			return false;
		}

		mChannels.append(channel);
	}

	if (mChannels.isEmpty()) {
		*error = QStringLiteral("no enabled scan channels");
		return false;
	}

	std::sort(mChannels.begin(), mChannels.end(),
		[](const ScanChannel &left, const ScanChannel &right) { return left.index < right.index; });

	auto offset = 0;
	for (auto i = 0; i < mChannels.size(); ++i) {
		auto &channel = mChannels[i];
		const auto bytes = channel.format.storageBits / 8;
		if (bytes <= 0) {
			*error = QStringLiteral("invalid storage size for ") + channel.name;
			return false;
		}

		offset = ((offset + bytes - 1) / bytes) * bytes;
		channel.offset = offset;
		offset += bytes;

		if (channel.isTimestamp) {
			mTimestampChannel = i;
		} else {
			mValueChannels.append(i);
		}
	}

	mRecordBytes = offset;
	return true;
}

bool TrikIIOFile::open()
{
	auto error = QString();
	if (!buildLayout(&error)) {
		QLOG_ERROR() << "IIO layout validation failed for" << mFileName << ":" << error;
		return false;
	}

	QLOG_INFO() << "Opening" << mFileName;
	mFd = ::open(mFileName.toStdString().c_str(), O_NONBLOCK | O_RDONLY | O_CLOEXEC);
	if (mFd == -1) {
		QLOG_ERROR() << QString("%1: open failed: %2").arg(mFileName, strerror(errno));
		return false;
	}

	mSocketNotifier.reset(new QSocketNotifier(mFd, QSocketNotifier::Read));
	connect(mSocketNotifier.data(), &QSocketNotifier::activated, this, &TrikIIOFile::readFile);
	mSocketNotifier->setEnabled(true);

	return true;
}

bool TrikIIOFile::close()
{
	if (mFd == -1) {
		return false;
	}

	QLOG_INFO() << "Closing" << mFileName;

	if (mSocketNotifier) {
		disconnect(mSocketNotifier.data(), &QSocketNotifier::activated, this, &TrikIIOFile::readFile);
		mSocketNotifier->setEnabled(false);
	}

	if (::close(mFd) != 0) {
		QLOG_ERROR() << QString("%1: close failed: %2").arg(mFileName, strerror(errno));
		return false;
	}

	mFd = -1;
	return true;
}

QString TrikIIOFile::fileName() const
{
	return mFileName;
}

bool TrikIIOFile::isOpened() const
{
	return mFd != -1;
}

QVector<trikHal::IIOChannelInfo> TrikIIOFile::channels() const
{
	QVector<trikHal::IIOChannelInfo> result;
	result.reserve(mValueChannels.size());
	for (const auto index : mValueChannels) {
		const auto &channel = mChannels.at(index);
		result.append(trikHal::IIOChannelInfo {channel.id, channel.index, channel.format});
	}

	return result;
}

int TrikIIOFile::channelValueIndex(IIOChanType type, IIOAxis axis) const
{
	for (auto position = 0; position < mValueChannels.size(); ++position) {
		const auto &channel = mChannels.at(mValueChannels.at(position));
		if (channel.id.type == type && channel.id.axis == axis) {
			return position;
		}
	}

	return -1;
}

int TrikIIOFile::recordBytes() const
{
	return mRecordBytes;
}

bool TrikIIOFile::hasTimestamp() const
{
	return mTimestampChannel >= 0;
}

void TrikIIOFile::readFile()
{
	mSocketNotifier->setEnabled(false);

	const auto recordBytes = std::max(mRecordBytes, 1);
	QVector<quint8> buffer(recordBytes * readRecordsPerChunk);

	for (;;) {
		const auto result = ::read(mFd, buffer.data(), buffer.size());
		if (result < 0) {
			if (errno == EAGAIN || errno == EINTR) {
				break;
			}

			QLOG_ERROR() << QString("%1: read failed: %2").arg(mFileName, strerror(errno));
			break;
		}

		if (result == 0) {
			break;
		}

		if (result % recordBytes != 0) {
			QLOG_WARN() << "IIO: read returned a non-record size:" << result;
		}

		for (auto offset = ssize_t {0}; offset + recordBytes <= result; offset += recordBytes) {
			processRecord(buffer.constData() + offset);
		}
	}

	mSocketNotifier->setEnabled(true);
}

void TrikIIOFile::processRecord(const quint8 *data)
{
	auto sample = IIOSample {};
	sample.values.reserve(mValueChannels.size());
	for (const auto index : mValueChannels) {
		const auto &channel = mChannels.at(index);
		sample.values.append(decodeValue(data + channel.offset, channel.format));
	}

	if (mTimestampChannel >= 0) {
		const auto &channel = mChannels.at(mTimestampChannel);
		sample.time = timeValFromNs(decodeTimestamp(data + channel.offset, channel.format));
	} else {
		sample.time = currentTime();
	}

	Q_EMIT newData(sample);
}
