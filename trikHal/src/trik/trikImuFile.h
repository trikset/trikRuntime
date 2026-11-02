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

#pragma once

#include <array>

#include <QSocketNotifier>
#include <QtCore/QScopedPointer>
#include <QtCore/QString>

#include "ImuFileInterface.h"

namespace trikHal {
namespace trik {

/// Format of one IIO scan channel.
struct ChannelFormat {
	/// Number of bits used to store the value.
	int storageBits = 0;

	/// Whether the stored value uses big-endian byte order.
	bool bigEndian = true;

	/// Number of low bits that must be discarded from the raw value.
	int shift = 0;

	bool operator==(const ChannelFormat &other) const
	{
		return storageBits == other.storageBits && bigEndian == other.bigEndian && shift == other.shift;
	}

	bool operator!=(const ChannelFormat &other) const
	{
		return !(*this == other);
	}
};

/// Implementation of a combined IMU IIO file.
///
/// Expects the full scan set: accel x/y/z, anglvel x/y/z, temp and timestamp.
/// The resulting record is 24 bytes: six 16-bit axes, temperature and an
/// 8-byte aligned timestamp.
///
/// The layout is read from sysfs once in open(). If the required channels are
/// not enabled or the computed record size is not 24 bytes, open() fails and
/// no samples are emitted.
class TrikImuFile : public ImuFileInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param fileName - file name (with path) of the IIO character device.
	explicit TrikImuFile(QString fileName);

	/// Destructor.
	~TrikImuFile() override;

	/// Opens the device and starts listening for samples.
	bool open() override;

	/// Closes the device and stops listening for samples.
	bool close() override;

	/// Returns the path of the IIO character device.
	QString fileName() const override;

	/// Returns true if the device is open.
	bool isOpened() const override;

private Q_SLOTS:
	/// Reads available bytes and processes complete samples.
	void readFile();

private:
	/// Reads scan_elements from sysfs and validates the combined IMU layout.
	bool validateLayout(QString *error);

	/// Parses one complete 24-byte record and emits newImuData().
	void processRecord(const uint8_t *data);

	int mFd = -1; ///< Descriptor of the opened IIO character device.
	const QString mFileName {}; ///< Path of the IIO character device.
	QScopedPointer<QSocketNotifier> mSocketNotifier {}; ///< Wakes readFile() when data is available.

	std::array<int, 3> mAccelOffsets {}; ///< Byte offsets of accel X/Y/Z in a sample.
	ChannelFormat mAccelFormat; ///< Format shared by all accelerometer axes.

	std::array<int, 3> mGyroOffsets {}; ///< Byte offsets of gyro X/Y/Z in a sample.
	ChannelFormat mGyroFormat; ///< Format shared by all gyroscope axes.

	int mTempOffset = 0; ///< Byte offset of the shared temperature channel.
	ChannelFormat mTempFormat; ///< Format of the shared temperature channel.

	int mTimestampOffset = 0; ///< Byte offset of the shared timestamp.
};

}
}
