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
#include <QtCore/QScopedPointer>

#include "fifoInterface.h"
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
	/// Constructor. Creates FIFO device by a description in config file.
	/// @param virtualPort - port in system-config.xml on which this FIFO file is configured.
	/// @param configurer - configurer object containing preparsed XML files with parameters.
	/// @param hardwareAbstraction - interface to underlying hardware or operating system capabilities of a robot.
	Fifo(const QString &virtualPort, const trikKernel::Configurer &configurer
			, const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	/// Constructor. Creates FIFO device programmatically by file name.
	/// @param fileName - name of a FIFO file.
	/// @param hardwareAbstraction - interface to underlying hardware or operating system capabilities of a robot.
	Fifo(const QString &fileName, const trikHal::HardwareAbstractionInterface &hardwareAbstraction);

	~Fifo() override;

	Status status() const override;

public slots:
	/// Reads data from this FIFO file, returning all available data as string.
	QString read() override;

	/// Returns true if FIFO has new data in it.
	bool hasData() const override;

private slots:
	void onNewData(const QString &data);
	void onReadError();

private:
	QScopedPointer<trikHal::FifoInterface> mFifo;

	/// Last line that was read from FIFO.
	QString mCurrent;

	/// State of a FIFO file as a device.
	DeviceState mState;
};

}
