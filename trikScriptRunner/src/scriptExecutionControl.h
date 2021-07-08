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
#include <QtCore/QStringList>
#include <QTimer>

#include <trikControl/brickInterface.h>
#include <trikScriptControlInterface.h>

namespace trikScriptRunner {

/// Script execution controller, provides related functions to scripts.
class TRIKSCRIPTRUNNER_EXPORT ScriptExecutionControl : public TrikScriptControlInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param brick - reference to trikControl::Brick instance.
	explicit ScriptExecutionControl(trikControl::BrickInterface *brick);

	~ScriptExecutionControl() override;

	/// Returns true if a script is in event-driven running mode, so it shall wait for events when script is executed.
	/// If it is false, script will exit immediately.
	bool isInEventDrivenMode() const override;

	/// Returns repacked RGB88 from 3 x uint8_t into int32_t image
	Q_INVOKABLE QVector<int32_t> getPhoto() override;

	/// Starts a new timer with given interval and returns reference to it.
	Q_INVOKABLE QObject *timer(int milliseconds) override;

	/// Waits given amount of time in milliseconds and returns.
	Q_INVOKABLE void wait(const int &milliseconds) override;

	/// Returns the number of milliseconds since 1970-01-01T00:00:00 UTC.
	Q_INVOKABLE qint64 time() const override;

	/// Returns random number from an interval [from, to].
	Q_INVOKABLE int random(int from, int to) const override;

	/// Execute given sh command.
	Q_INVOKABLE void system(const QString &command, bool synchronously = false) override;

	/// Appends given text to the end of a file.
	Q_INVOKABLE void writeToFile(const QString &file, const QString &text) override;

	/// Appends given bytes to the end of a file.
	Q_INVOKABLE void writeData(const QString &file, const QVector<uint8_t> &bytes) override;

	/// Reads all lines from a text file and returns it as a list of strings.
	Q_INVOKABLE QStringList readAll(const QString &file) const override;

	/// Removes a file.
	Q_INVOKABLE void removeFile(const QString &file) override;

	/// Counts time interval between two packed data of time using TimeVal
	Q_INVOKABLE int timeInterval(int packedTimeLeft, int packedTimeRight) override;

public slots:
	/// Starts event loop for script.
	void run() override;

	/// Aborts script execution.
	void quit() override;

	/// Resets script execution state, clearing all flags and stopping all timers.
	void reset() override;

signals:
	/// Emitted when script requested system to abort execution.
	void quitSignal();

	/// To be connected to quit() slot of local event loops that are used for waiting.
	void stopWaiting();

	/// Requests sending a message to a desktop.
	void textInStdOut(const QString &text);

private:
	QList<QTimer *> mTimers; // Has ownership.
	trikControl::BrickInterface *mBrick {}; //Does not have ownership


	/// True, if a system is in event-driven running mode, so it shall wait for events when script is executed.
	/// If it is false, script will exit immediately.
	bool mInEventDrivenMode = false;
};

}
