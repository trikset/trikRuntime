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
#include <QtCore/QScopedPointer>
#include <QSocketNotifier>
#include <lidarInterface.h>
#include <qsemaphore.h>

#include "deviceState.h"
#include "fifo.h"

#include <trikHal/fifoInterface.h>
#include <trikHal/hardwareAbstractionInterface.h>
#include <trikKernel/configurer.h>

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
	void onNewData(const QVector<uint8_t> &data);

	/// Called when there is new data on a FIFO.
	void readFile();

private:
	int countMean(const int i, const int meanWindow) const;

	void processBuffer();

	void processData(const QVector<uint8_t> &data);

	bool checkProtocol(const QVector<uint8_t> &data, uint start);
	bool checkChecksum(const QVector<uint8_t> &data, uint start, uint size);

	QScopedPointer<Fifo> mFifo; // Has ownership
	const QString mFifoFileName;
	int mFileDescriptor;
	const trikHal::HardwareAbstractionInterface &mHardwareAbstraction;

	QVector<int> mResult;

	QMutex mBufferLock;
	QVector<uint8_t> mBuffer;

	/// Notifier for FIFO file that emits a signal when something is changed in it.
	QScopedPointer<QSocketNotifier> mSocketNotifier;

	/// Releases when init() is finished
	QSemaphore mWaitForInit {1};
};
}
