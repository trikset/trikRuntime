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

#include <QtCore/QScopedPointer>

#include "fifoInterface.h"

class QSocketNotifier;

namespace trikHal {
namespace trik {

/// Real implementation of FIFO.
class TrikFifo : public FifoInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param fileName - name of a FIFO file.
	explicit TrikFifo(const QString &fileName);

	~TrikFifo() override;

	bool open() override;
	bool close() override;
	QString fileName() override;

private slots:
	/// Called when there is new data on a FIFO.
	void readFile();

private:
	/// Name of a FIFO file.
	const QString mFileName;

	/// FIFO file descriptor.
	int mFileDescriptor;

	/// Notifier for FIFO file that emits a signal when something is changed in it.
	QScopedPointer<QSocketNotifier> mSocketNotifier;

	/// Buffer with current line being read from FIFO.
	QString mBuffer;
};

}
}
