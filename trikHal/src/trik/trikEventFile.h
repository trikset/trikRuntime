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

#include "eventFileInterface.h"

class QEventLoop;
class QSocketNotifier;

namespace trikHal {
namespace trik {

/// Real implementation of event file.
class TrikEventFile : public EventFileInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param fileName - file name (with path, relative or absolute) of an event file.
	/// @param thread - background thread where all socket events will be processed.
	TrikEventFile(const QString &fileName, QThread &thread);

	~TrikEventFile() override;

	bool open() override;
	bool close() override;
	void cancelWaiting() override;
	QString fileName() const override;
	bool isOpened() const override;

private slots:
	/// Tries to open event file and if opened successfully stops waiting event loop.
	void tryOpenEventFile();

	/// Called when there is a new event in a file.
	void readFile();

private:
	/// Low-level file descriptor for event file.
	int mEventFileDescriptor = -1;

	/// File name of an event file.
	const QString mFileName;

	/// Background thread where all socket events will be processed.
	QThread &mThread;

	/// Waiting loop that is used to retry opening attempt if event file does not exist yet (some drivers take some
	/// time to init and create event file).
	QScopedPointer<QEventLoop> mInitWaitingLoop;

	/// Socket notifer that is used to listen for events in a file.
	QScopedPointer<QSocketNotifier> mSocketNotifier;
};

}
}
