/* Copyright 2013 Yurii Litvinov
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
#include <QtCore/QThread>
#include <QtScript/QScriptEngine>

#include <trikControl/brick.h>

namespace trikScriptRunner
{

/// Worker object to be run in a separate thread for Qt Script execution. QScriptEngine calls ProcessEvents too
/// infrequently even when ProcessEventsInterval is set to 1 ms, so there is a need for separate threads to
/// run a script and listen for incoming connections.
class ScriptEngineWorker : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param brick - reference to trikControl::Brick instance.
	/// @param startDirPath - path to the directory from which the application was executed.
	ScriptEngineWorker(trikControl::Brick &brick, QString const &startDirPath);

	/// Stops script execution and resets execution state (including script engine and trikControl itself). Can be
	/// called from another thread.
	void reset();

signals:
	/// Emitted when current script execution is completed or is aborted by reset() call.
	/// @param error - localized error message or empty string.
	void completed(QString const &error);

public slots:
	/// Initializes script engine and creates its own trikControl instance.
	void init();

	/// Executes given script.
	/// @param script - script to execute.
	/// @param inEventDrivenMode - shall this script be executed in event-driven mode, i.e. not emit completed() signal
	///        when it is finished.
	void run(QString const &script, bool inEventDrivenMode);

private slots:
	/// Abort script execution.
	void onScriptRequestingToQuit();

	/// Kill old script engine, create and reinit a new one.
	void resetScriptEngine();

private:
	void onScriptEvaluated();

	// Has ownership. No smart pointers here because we need to do manual memory managment
	// due to complicated mEngine lifecycle (see .cpp for more details).
	QScriptEngine *mEngine;

	trikControl::Brick &mBrick;

	QString const mStartDirPath;
	QThread *mGuiThread;  // Does not have ownership.
};

}
