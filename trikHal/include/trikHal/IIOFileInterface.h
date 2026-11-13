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

#include <QtCore/QObject>
#include <QtCore/QVector>

#include <trikKernel/timeVal.h>

#include <trikHal/IIOChannels.h>
#include <trikHal/IIOChannelFormat.h>
#include <trikHal/trikHalDeclSpec.h>

namespace trikHal {

/// Description of one enabled IIO scan channel.
struct IIOChannelInfo {
	/// Channel type and axis.
	IIOChannelId id;

	/// Scan index assigned by the IIO subsystem.
	int index = -1;

	/// Storage format of the channel.
	IIOChannelFormat format;
};

/// One decoded sample read from an IIO character device.
struct IIOSample {
	/// Decoded values of all non-timestamp channels, in the same order as
	/// IIOFileInterface::channels().
	QVector<int> values;

	/// Sample timestamp (hardware timestamp if the device provides one).
	trikKernel::TimeVal time {0, 0};
};

/// Generic IIO character device abstraction.
///
/// Knows nothing about particular sensor types: it reads the scan layout from
/// sysfs, decodes any enabled channel according to its declared format, and
/// emits decoded samples. Choosing which channels are needed and validating
/// their presence is the responsibility of the client (for example BoardImu).
class TRIKHAL_EXPORT IIOFileInterface : public QObject
{
	Q_OBJECT

public:
	~IIOFileInterface() override = default;

	/// Opens the IIO device and starts listening for samples.
	virtual bool open() = 0;

	/// Closes the IIO device and stops listening for samples.
	virtual bool close() = 0;

	/// Returns the path of the IIO character device.
	virtual QString fileName() const = 0;

	/// Returns true if the device is currently open.
	virtual bool isOpened() const = 0;

	/// Describes the enabled non-timestamp channels, in the same order as
	/// IIOSample::values.
	virtual QVector<IIOChannelInfo> channels() const = 0;

	/// Returns the position of the requested channel in IIOSample::values, or
	/// -1 if the channel is not present.
	virtual int channelValueIndex(IIOChanType type, IIOAxis axis) const = 0;

	/// Returns the byte size of one sample record in the IIO buffer.
	virtual int recordBytes() const = 0;

	/// Returns true if the device provides a hardware timestamp channel.
	virtual bool hasTimestamp() const = 0;

Q_SIGNALS:
	/// Emitted for every complete sample read from the device.
	void newData(const trikHal::IIOSample &sample);
};

}

Q_DECLARE_METATYPE(trikHal::IIOSample)
