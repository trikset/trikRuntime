/* Copyright 2015 CyberTech Labs Ltd.
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
#include <QtCore/QScopedPointer>

#include <QMutex>
#include <QWaitCondition>

#include "fifoInterface.h"
#include "fifoworker.h"
#include "deviceState.h"

namespace trikKernel {
class Configurer;
}

namespace trikHal {
class HardwareAbstractionInterface;
class FifoInterface;
}

namespace trikControl {

/// Class that represents linux FIFO file, which is commonly used by various sensors.
class Fifo: public FifoInterface
{
	Q_OBJECT

public:
	/// Constructor. Creates FIFO worker and moves it in a separate thread.
	/// @param virtualPort - port in system-config.xml on which this FIFO file is configured.
	/// @param configurer - configurer object containing preparsed XML files with parameters.
	/// @param hardwareAbstraction - interface to underlying hardware or operating system capabilities of a robot.
	Fifo(const QString &virtualPort, const trikKernel::Configurer &configurer
			, const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	/// Constructor. Creates FIFO worker and moves it in a separate thread.
	/// @param fileName - name of a FIFO file.
	/// @param hardwareAbstraction - interface to underlying hardware or operating system capabilities of a robot.
	Fifo(const QString &fileName, const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	~Fifo() override;

	Status status() const override;

	/// Reads line from this FIFO file, returning all available data as string.
	Q_INVOKABLE QString read() override;
	/// Reads data from this FIFO file, returning all available data as string.
	Q_INVOKABLE QVector<uint8_t> readRaw() override;

	/// Returns true if FIFO has new line in it.
	Q_INVOKABLE bool hasLine() const override;
	/// Returns true if FIFO has new bytes in it.
	Q_INVOKABLE bool hasData() const override;

private:
	/// Worker object that handles sensor in separate thread.
	FifoWorker *mFifoWorker; // Has ownership.

	/// Worker thread.
	QThread mWorkerThread;
};

}
