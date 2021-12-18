/* Copyright 2020 CyberTech Labs Ltd.
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

#include "deviceState.h"

#include <trikHal/fifoInterface.h>
#include <trikHal/hardwareAbstractionInterface.h>

#include <qsemaphore.h>

namespace trikControl {

/// Worker object that processes FIFO output and updates stored reading. Meant to be executed in separate
/// thread.
class FifoWorker : public QObject, public DeviceInterface
{
	Q_OBJECT

public:
	/// Constructor. Creates FIFO device programmatically by file name.
	/// @param fileName - name of a FIFO file.
	/// @param hardwareAbstraction - interface to underlying hardware or operating system capabilities of a robot.
	FifoWorker(const QString &fileName
			   , const trikHal::HardwareAbstractionInterface &hardwareAbstraction);
	~FifoWorker();

	Status status() const override;

public slots:
	/// Work with DeviceState and connections.
	void init();
	/// Reads line from this FIFO file, returning all available data as string.
	QString read();
	/// Reads data from this FIFO file, returning all available data as string.
	QVector<uint8_t> readRaw();

	/// Returns true if FIFO has new line in it.
	bool hasLine() const;
	/// Returns true if FIFO has new bytes in it.
	bool hasData() const;

	/// Blocks the Thread with QSemaphore until init() method releases it.
	void waitUntilInited();

signals:
	/// Emitted once per each text line that arrives to FIFO.
	void newLine(const QString &data);
	/// Emitted when new bytes have arrived to FIFO file.
	void newData(const QVector<uint8_t> &data);

private slots:
	void onNewLine(const QString &line);
	void onNewData(const QVector<uint8_t> &data);
	void onReadError();

private:
	const QString mFifoFileName;
	const trikHal::HardwareAbstractionInterface &mHardwareAbstraction;
	QScopedPointer<trikHal::FifoInterface> mFifo;

	/// Last line that was read from FIFO.
	QString mCurrentLine;

	/// Last data that was read from FIFO.
	QVector<uint8_t> mCurrentData;

	/// Lock for mCurrent
	mutable QReadWriteLock mCurrentLock;

	/// State of a FIFO file as a device.
	DeviceState mState;

	/// Releases when init() is finished
	QSemaphore mWaitForInit {1};
};

}
