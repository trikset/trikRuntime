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

#pragma once

#include <QScopedPointer>
#include <QSocketNotifier>
#include <QString>
#include <QVector>

#include "IIOFileInterface.h"

namespace trikHal {
namespace trik {

/// Generic implementation of an IIO character device.
///
/// Reads the scan layout from sysfs (`scan_elements`), decodes any enabled
/// channel according to its declared `_type` format and emits decoded samples.
/// Knows nothing about particular sensor types.
class TrikIIOFile : public IIOFileInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param fileName - file name (with path) of the IIO character device.
	explicit TrikIIOFile(QString fileName);

	/// Destructor.
	~TrikIIOFile() override;

	bool open() override;
	bool close() override;
	QString fileName() const override;
	bool isOpened() const override;

	QVector<IIOChannelInfo> channels() const override;
	int channelValueIndex(IIOChanType type, IIOAxis axis) const override;
	int recordBytes() const override;
	bool hasTimestamp() const override;

private Q_SLOTS:
	/// Called when there is new data in the device.
	void readFile();

private:
	/// One resolved scan channel.
	struct ScanChannel {
		IIOChannelId id;
		QString name;
		int index = -1;
		int offset = 0;
		IIOChannelFormat format;
		bool isTimestamp = false;
	};

	/// Reads scan_elements from sysfs and computes the record layout.
	bool buildLayout(QString *error);

	/// Parses and emits one complete record.
	void processRecord(const quint8 *data);

	int mFd = -1;
	const QString mFileName;
	QScopedPointer<QSocketNotifier> mSocketNotifier;

	QVector<ScanChannel> mChannels; ///< All channels, sorted by scan index.
	QVector<int> mValueChannels; ///< Indices in mChannels of non-timestamp channels.
	int mRecordBytes = 0; ///< Byte size of one sample record.
	int mTimestampChannel = -1; ///< Index in mChannels of the timestamp channel, or -1.
};

}
}
