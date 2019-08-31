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

#include <QtCore/QList>
#include <QtCore/QTimer>
#include <QtCore/QStringList>

namespace trikScriptRunner {

/// Script execution controller, provides related functions to scripts.
class ScriptExecutionControl : public QObject
{
	Q_OBJECT

public:
	~ScriptExecutionControl() override;

	/// Returns true if a script is in event-driven running mode, so it shall wait for events when script is executed.
	/// If it is false, script will exit immediately.
	bool isInEventDrivenMode() const;

	/// Starts a new timer with given interval and returns reference to it.
	Q_INVOKABLE QTimer *timer(int milliseconds);

	/// Waits given amount of time in milliseconds and returns.
	Q_INVOKABLE void wait(const int &milliseconds);

	/// Returns the number of milliseconds since 1970-01-01T00:00:00 UTC.
	Q_INVOKABLE qint64 time() const;

	/// Returns random number from an interval [from, to].
	Q_INVOKABLE int random(int from, int to) const;

	/// Execute given sh command.
	Q_INVOKABLE void system(const QString &command, bool synchronously = false);

	/// Appends given text to the end of a file.
	Q_INVOKABLE void writeToFile(const QString &file, const QString &text);

	/// Reads all lines from a text file and returns it as a list of strings.
	Q_INVOKABLE QStringList readAll(const QString &file) const;

	/// Removes a file.
	Q_INVOKABLE void removeFile(const QString &file);

public slots:
	/// Starts event loop for script.
	void run();

	/// Aborts script execution.
	void quit();

	/// Resets script execution state, clearing all flags and stopping all timers.
	void reset();

signals:
	/// Emitted when script requested system to abort execution.
	void quitSignal();

	/// To be connected to quit() slot of local event loops that are used for waiting.
	void stopWaiting();

	/// Requests sending a message to a desktop.
	void sendMessage(const QString &text);

private:
	QList<QTimer *> mTimers; // Has ownership.

	/// True, if a system is in event-driven running mode, so it shall wait for events when script is executed.
	/// If it is false, script will exit immediately.
	bool mInEventDrivenMode = false;
};

}
