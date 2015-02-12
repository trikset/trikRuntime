/* Copyright 2013 - 2015 Yurii Litvinov and CyberTech Labs Ltd.
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

#include <trikControl/brickInterface.h>
#include <trikNetwork/mailboxInterface.h>
#include <trikNetwork/gamepadInterface.h>

#include "scriptExecutionControl.h"
#include "threading.h"

namespace trikScriptRunner
{

/// Worker object to be run in a separate thread for Qt Script execution. QScriptEngine calls ProcessEvents too
/// infrequently even when ProcessEventsInterval is set to 1 ms, so there is a need for separate threads to
/// run a script and listen for incoming connections.
class ScriptEngineWorker : public QObject
{
	Q_OBJECT

public:
	enum State {
		ready
		, starting
		, resetting
		, running
	};

	/// Constructor.
	/// @param brick - reference to trikControl::Brick instance.
	/// @param mailbox - mailbox object used to communicate with other robots.
	/// @param gamepad - gamepad object used to interact with TRIK Gamepad on Android device.
	/// @param scriptControl - reference to script execution control object.
	/// @param startDirPath - path to the directory from which the application was executed.
	ScriptEngineWorker(trikControl::BrickInterface &brick
			, trikNetwork::MailboxInterface * const mailbox
			, trikNetwork::GamepadInterface * const gamepad
			, ScriptExecutionControl &scriptControl
			, QString const &startDirPath);

	/// Create and initizlize a new script engine.
	QScriptEngine *createScriptEngine();

signals:
	/// Emitted when current script execution is completed or is aborted by reset() call.
	/// @param error - localized error message or empty string.
	/// @param scriptId - unique identifier of a script completed
	void completed(QString const &error, int scriptId);

	/// Emitted when evaluation is being interrupted. Used for stopping child threads when robot reset is requested.
	void abortEvaluation();

public slots:
	/// Stops script execution and resets execution state (including script engine and trikControl itself). Can be
	/// called from another thread.
	void reset();

	/// Executes given script.
	/// @param script - script to execute.
	/// @param inEventDrivenMode - shall this script be executed in event-driven mode, i.e. not emit completed() signal
	///        when it is finished.
	/// @param function - the name of the function execution must start with. If empty then the script will be
	/// evaluated as-is, else function call will be appended to @arg script.
	void run(QString const &script, int scriptId);

	void runDirect(QString const &command, int scriptId);

	/// Plays "beep" sound.
	void brickBeep();

signals:
	/// Emitted when new script is started.
	/// @param scriptId - unique identifier assigned to a newly started script.
	void startedScript(int scriptId);

private slots:
	/// Abort script execution.
	void onScriptRequestingToQuit();

	void doRun(const QString &script);

	void doRunDirect(const QString &command);

private:
	void startScriptEvaluation(int scriptId);

	trikControl::BrickInterface &mBrick;
	trikNetwork::MailboxInterface * const mMailbox;  // Does not have ownership.
	trikNetwork::GamepadInterface * const mGamepad;  // Does not have ownership.
	ScriptExecutionControl &mScriptControl;
	Threading mThreadingVariable;
	QScriptEngine *mDirectScriptsEngine;
	QString const mStartDirPath;
	int mScriptId;
	State mState;
};

}
