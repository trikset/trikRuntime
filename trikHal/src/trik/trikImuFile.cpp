/* Copyright 2026 CyberTech Labs Ltd.
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

#include "trikImuFile.h"

#include <algorithm>
#include <cerrno>
#include <cstring>
#include <utility>

#include <fcntl.h>
#include <unistd.h>

#include <QtCore/QFileInfo>
#include <QtCore/QVector>

#include <trikKernel/fileUtils.h>
#include <trikKernel/timeVal.h>

#include <QsLog.h>

namespace {

constexpr int expectedRecordBytes = 24;
constexpr int readBufferBytes = expectedRecordBytes * 10;
constexpr int timestampBytes = static_cast<int>(sizeof(qint64));

enum class ChannelKind {
	AccelX,
	AccelY,
	AccelZ,
	GyroX,
	GyroY,
	GyroZ,
	Temp,
	Timestamp,
};

struct ChannelDef {
	const char *sysfsName;
	ChannelKind kind;
};

constexpr ChannelDef channelDefs[] = {
	{"in_accel_x", ChannelKind::AccelX},
	{"in_accel_y", ChannelKind::AccelY},
	{"in_accel_z", ChannelKind::AccelZ},
	{"in_anglvel_x", ChannelKind::GyroX},
	{"in_anglvel_y", ChannelKind::GyroY},
	{"in_anglvel_z", ChannelKind::GyroZ},
	{"in_temp", ChannelKind::Temp},
	{"in_timestamp", ChannelKind::Timestamp},
};

struct ScanChannel {
	QString name {};
	ChannelKind kind = ChannelKind::AccelX;
	int index = -1;
	int offset = 0;
	trikHal::trik::ChannelFormat format;
};

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

bool parseScanType(const QString &type, trikHal::trik::ChannelFormat *format)
{
	const auto slashPos = type.indexOf(QLatin1Char('/'));
	const auto shiftPos = type.indexOf(QStringLiteral(">>"));
	if (slashPos < 0 || shiftPos < 0 || shiftPos <= slashPos) {
		return false;
	}

	auto storageOk = false;
	auto shiftOk = false;
	const auto storageBits = type.mid(slashPos + 1, shiftPos - slashPos - 1).toInt(&storageOk);
	const auto shift = type.mid(shiftPos + 2).toInt(&shiftOk);
	if (!storageOk || !shiftOk || storageBits <= 0 || shift < 0) {
		return false;
	}

	format->storageBits = storageBits;
	format->bigEndian = type.startsWith(QStringLiteral("be:"));
	format->shift = shift;
	return true;
}

bool readChannels(const QString &scanDir, QVector<ScanChannel> *channels, QString *error)
{
	for (const auto &def : channelDefs) {
		const auto name = QString::fromLatin1(def.sysfsName);

		auto enabled = QString();
		if (!readSysfsValue(scanDir + QLatin1Char('/') + name + QStringLiteral("_en"), &enabled)
			|| enabled != QLatin1String("1")) {
			*error = QStringLiteral("channel %1 is not enabled").arg(name);
			return false;
		}

		auto indexText = QString();
		if (!readSysfsValue(scanDir + QLatin1Char('/') + name + QStringLiteral("_index"), &indexText)) {
			*error = QStringLiteral("can not read scan index for %1").arg(name);
			return false;
		}

		auto indexOk = false;
		const auto index = indexText.toInt(&indexOk);
		if (!indexOk) {
			*error = QStringLiteral("invalid scan index for %1").arg(name);
			return false;
		}

		auto typeText = QString();
		if (!readSysfsValue(scanDir + QLatin1Char('/') + name + QStringLiteral("_type"), &typeText)) {
			*error = QStringLiteral("can not read scan type for %1").arg(name);
			return false;
		}

		auto channel = ScanChannel {};
		channel.name = name;
		channel.kind = def.kind;
		channel.index = index;
		if (!parseScanType(typeText, &channel.format)) {
			*error = QStringLiteral("unsupported scan type '%1' for %2").arg(typeText, name);
			return false;
		}

		channels->append(channel);
	}

	return true;
}

bool computeOffsets(QVector<ScanChannel> *channels, QString *error)
{
	std::sort(channels->begin(), channels->end(),
		[](const ScanChannel &left, const ScanChannel &right) { return left.index < right.index; });

	auto offset = 0;
	for (auto &channel : *channels) {
		const auto bytes = channel.format.storageBits / 8;
		if (bytes <= 0) {
			*error = QStringLiteral("invalid storage size for %1").arg(channel.name);
			return false;
		}

		offset = ((offset + bytes - 1) / bytes) * bytes;
		channel.offset = offset;
		offset += bytes;
	}

	if (offset != expectedRecordBytes) {
		*error = QStringLiteral("unexpected scan size: %1, expected %2").arg(offset).arg(expectedRecordBytes);
		return false;
	}

	return true;
}

const ScanChannel *findChannel(const QVector<ScanChannel> &channels, ChannelKind kind)
{
	for (const auto &channel : channels) {
		if (channel.kind == kind) {
			return &channel;
		}
	}

	return nullptr;
}

trikKernel::TimeVal timeValFromNs(qint64 timestampNs)
{
	const auto timestampMcsec = static_cast<quint64>(timestampNs) / 1000;
	return {static_cast<int>(timestampMcsec / 1000000), static_cast<int>(timestampMcsec % 1000000)};
}

}

using namespace trikHal::trik;

TrikImuFile::TrikImuFile(QString fileName)
	: mFileName(std::move(fileName))
{
}

TrikImuFile::~TrikImuFile()
{
	close();
}

bool TrikImuFile::open()
{
	auto error = QString();
	if (!validateLayout(&error)) {
		QLOG_ERROR() << "IMU layout validation failed for" << mFileName << ":" << error;
		return false;
	}

	QLOG_INFO() << "Opening" << mFileName;
	mFd = ::open(mFileName.toStdString().c_str(), O_NONBLOCK | O_RDONLY | O_CLOEXEC);
	if (mFd == -1) {
		QLOG_ERROR() << QString("%1: open failed: %2").arg(mFileName, strerror(errno));
		return false;
	}

	mSocketNotifier.reset(new QSocketNotifier(mFd, QSocketNotifier::Read));
	connect(mSocketNotifier.data(), &QSocketNotifier::activated, this, &TrikImuFile::readFile);
	mSocketNotifier->setEnabled(true);

	return true;
}

bool TrikImuFile::close()
{
	if (mFd == -1) {
		return false;
	}

	QLOG_INFO() << "Closing" << mFileName;

	if (mSocketNotifier) {
		disconnect(mSocketNotifier.data(), &QSocketNotifier::activated, this, &TrikImuFile::readFile);
		mSocketNotifier->setEnabled(false);
	}

	if (::close(mFd) != 0) {
		QLOG_ERROR() << QString("%1: close failed: %2").arg(mFileName, strerror(errno));
		return false;
	}

	mFd = -1;
	return true;
}

QString TrikImuFile::fileName() const
{
	return mFileName;
}

bool TrikImuFile::isOpened() const
{
	return mFd != -1;
}

bool TrikImuFile::validateLayout(QString *error)
{
	const auto deviceName = QFileInfo(mFileName).fileName();
	const auto scanDir = QStringLiteral("/sys/bus/iio/devices/") + deviceName + QStringLiteral("/scan_elements");

	QVector<ScanChannel> channels;
	if (!readChannels(scanDir, &channels, error)) {
		return false;
	}

	if (!computeOffsets(&channels, error)) {
		return false;
	}

	const auto *timestamp = findChannel(channels, ChannelKind::Timestamp);
	if (!timestamp || timestamp->format.storageBits != 64
		|| timestamp->offset != expectedRecordBytes - timestampBytes) {
		*error = QStringLiteral("unexpected timestamp layout");
		return false;
	}

	const std::array<ChannelKind, 3> accelKinds {ChannelKind::AccelX, ChannelKind::AccelY, ChannelKind::AccelZ};
	const std::array<ChannelKind, 3> gyroKinds {ChannelKind::GyroX, ChannelKind::GyroY, ChannelKind::GyroZ};

	auto accelFormat = ChannelFormat {};
	auto gyroFormat = ChannelFormat {};
	for (auto i = std::size_t {0}; i < accelKinds.size(); ++i) {
		const auto *accel = findChannel(channels, accelKinds[i]);
		const auto *gyro = findChannel(channels, gyroKinds[i]);
		if (!accel || !gyro) {
			*error = QStringLiteral("missing IMU axis channel");
			return false;
		}

		if (accel->format.storageBits != 16 || gyro->format.storageBits != 16) {
			*error = QStringLiteral("IMU axes must be 16-bit");
			return false;
		}

		if (i == 0) {
			accelFormat = accel->format;
			gyroFormat = gyro->format;
		} else if (accel->format != accelFormat || gyro->format != gyroFormat) {
			*error = QStringLiteral("IMU axes have different scan types");
			return false;
		}

		mAccelOffsets[i] = accel->offset;
		mGyroOffsets[i] = gyro->offset;
	}

	mAccelFormat = accelFormat;
	mGyroFormat = gyroFormat;

	const auto *temp = findChannel(channels, ChannelKind::Temp);
	if (!temp || (temp->format.storageBits != 8 && temp->format.storageBits != 16)) {
		*error = QStringLiteral("unexpected temperature channel");
		return false;
	}

	mTempOffset = temp->offset;
	mTempFormat = temp->format;
	mTimestampOffset = timestamp->offset;

	return true;
}

void TrikImuFile::readFile()
{
	mSocketNotifier->setEnabled(false);

	uint8_t buffer[readBufferBytes];
	for (;;) {
		const auto result = ::read(mFd, buffer, sizeof(buffer));
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

		if (result % expectedRecordBytes != 0) {
			QLOG_WARN() << "IMU: read returned a non-record size:" << result;
		}

		for (auto offset = ssize_t {0}; offset + expectedRecordBytes <= result; offset += expectedRecordBytes) {
			processRecord(buffer + offset);
		}
	}

	mSocketNotifier->setEnabled(true);
}

void TrikImuFile::processRecord(const uint8_t *data)
{
	const auto readAxis = [](const uint8_t *axisData, const ChannelFormat &format) -> int {
		const auto raw = format.bigEndian ? static_cast<int16_t>((axisData[0] << 8) | axisData[1])
		                                  : static_cast<int16_t>((axisData[1] << 8) | axisData[0]);
		return static_cast<int>(raw >> format.shift);
	};

	auto temperature = 0;
	if (mTempFormat.storageBits == 16) {
		temperature = readAxis(data + mTempOffset, mTempFormat);
	} else {
		temperature = static_cast<int>(static_cast<int8_t>(data[mTempOffset]) >> mTempFormat.shift);
	}

	auto timestampNs = qint64 {0};
	memcpy(&timestampNs, data + mTimestampOffset, sizeof(timestampNs));

	auto sample = ImuSample {};
	sample.accel.resize(3);
	sample.gyro.resize(3);
	for (auto i = 0; i < 3; ++i) {
		sample.accel[i] = readAxis(data + mAccelOffsets[i], mAccelFormat);
		sample.gyro[i] = readAxis(data + mGyroOffsets[i], mGyroFormat);
	}

	sample.temperature = temperature;
	sample.time = timeValFromNs(timestampNs);

	Q_EMIT newImuData(sample);
}
