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
#include <QtCore/QThread>
#include <QSemaphore>
#include <QSerialPort>
#include <QScopedArrayPointer>
#include <stdint.h>

#include "deviceState.h"

#include <trikHal/hardwareAbstractionInterface.h>

namespace trikControl {

/// Worker object that processes Lidar output and updates stored reading. Meant to be executed in separate
/// thread.
class LidarWorker : public QObject, public DeviceInterface
{
	Q_OBJECT
public:
	/// Constructor.
	/// @param fileName - name of a FIFO file.
	/// @param hardwareAbstraction - interface to underlying hardware or operating system capabilities of a robot.
	explicit LidarWorker(const QString &fileName, const trikHal::HardwareAbstractionInterface &hardwareAbstraction);
	~LidarWorker();

	Status status() const override;
	/// Inits Fifo
	void init();

	/// Blocks the Thread with QSemaphore until init() method releases it.
	void waitUntilInited();

	/// Calculates the arithmetic average of the lidar values and outputs in shorter form.
	Q_INVOKABLE QVector<int> read() const;

	/// Returning all available raw data.
	Q_INVOKABLE QVector<int> readRaw() const;

private slots:
	/// Called when there is new data on serial port
	void readData();

private:
	int countMean(const int i, const int meanWindow) const;

	void processData(const void *p);

	bool checkChecksum(const uint8_t *data, size_t size);

	/// serial port QT object
	QSerialPort mSerial;

	/// buffer for unparsed raw data chunks from serial port
	QScopedArrayPointer<uint8_t> mLidarChunk;
	size_t mLidarChunkBytes {};
	bool mFlagHunt {true};

	/// processed data from lidar
	QVector<int> mResult;

	/// device state
	DeviceState mState;

	/// Releases when init() is finished
	QSemaphore mWaitForInit {1};
};
}
