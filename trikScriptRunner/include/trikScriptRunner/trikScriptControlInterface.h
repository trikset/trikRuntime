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

#include <trikControl/brickInterface.h>

#include "trikScriptRunnerDeclSpec.h"

namespace trikScriptRunner {

/// Script execution controller, provides related functions to scripts.
class TRIKSCRIPTRUNNER_EXPORT TrikScriptControlInterface : public QObject
{
	Q_OBJECT

public:
	/// Registers required metatypes
	TrikScriptControlInterface();

	/// Returns true if a script is in event-driven running mode, so it shall wait for events when script is executed.
	/// If it is false, script will exit immediately.
	virtual bool isInEventDrivenMode() const = 0;

	/// Returns repacked RGB88 from 3 x uint8_t into int32_t image
	Q_INVOKABLE virtual QVector<int32_t> getPhoto() = 0;

	/// Starts a new timer with given interval and returns reference to it.
	/// QObject - hack to be able to inherit together QTimer and AbstractTimer from the studio
	Q_INVOKABLE virtual QObject *timer(int milliseconds) = 0;

	/// Waits given amount of time in milliseconds and returns.
	Q_INVOKABLE virtual void wait(const int &milliseconds) = 0;

	/// Returns the number of milliseconds since 1970-01-01T00:00:00 UTC.
	Q_INVOKABLE virtual qint64 time() const = 0;

	/// Returns random number from an interval [from, to].
	Q_INVOKABLE virtual int random(int from, int to) const = 0;

	/// Execute given sh command.
	Q_INVOKABLE virtual void system(const QString &command, bool synchronously = false) = 0;

	/// Appends given text to the end of a file.
	Q_INVOKABLE virtual void writeToFile(const QString &file, const QString &text) = 0;

	/// Appends given bytes to the end of a file.
	Q_INVOKABLE virtual void writeData(const QString &file, const QVector<uint8_t> &bytes) = 0;

	/// Reads all lines from a text file and returns it as a list of strings.
	Q_INVOKABLE virtual QStringList readAll(const QString &file) const = 0;

	/// Removes a file.
	Q_INVOKABLE virtual void removeFile(const QString &file) = 0;

	/// Counts time interval between two packed data of time using TimeVal
	Q_INVOKABLE virtual int timeInterval(int packedTimeLeft, int packedTimeRight) = 0;

public slots:
	/// Starts event loop for script.
	virtual void run() = 0;

	/// Aborts script execution.
	virtual void quit() = 0;

	/// Resets script execution state, clearing all flags and stopping all timers.
	virtual void reset() = 0;

signals:
	/// Emitted when script requested system to abort execution.
	void quitSignal();

	/// To be connected to quit() slot of local event loops that are used for waiting.
	void stopWaiting();

	/// Requests sending a message to a desktop.
	void textInStdOut(const QString &text);
};

}
