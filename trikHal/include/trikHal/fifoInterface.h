/* Copyright 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include <QtCore/QString>
#include <QtCore/QObject>
#include <trikHal/trikHalDeclSpec.h>

namespace trikHal {

/// Abstraction of Linux FIFO used to receive events. To write to FIFO, use OutputDeviceFileInterface.
class TRIKHAL_EXPORT FifoInterface : public QObject
{
	Q_OBJECT

public:
	/// Opens FIFO file and starts listening for events.
	/// @returns true, if opened successfully.
	virtual bool open() = 0;

	/// Closes FIFO file and stops listening for events.
	/// @returns true, if closed successfully.
	virtual bool close() = 0;

	/// Returns file name of a FIFO file.
	virtual QString fileName() = 0;

signals:
	/// Emitted when new data is read from FIFO.
	void newData(QVector<uint8_t> data);
	/// Emitted for each line from FIFO.
	void newLine(QString data);
	/// Emitted when something is wrong with opened FIFO file and reading failed.
	/// It may be caused, for example, by another process reading from the same FIFO, it may partially read data.
	void readError();
};

}
