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

#include <functional>

#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtScript/QScriptEngine>

#include <trikControl/brickInterface.h>
#include <trikNetwork/mailboxInterface.h>

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
	/// Constructor.
	/// @param brick - reference to trikControl::Brick instance.
	/// @param mailbox - mailbox object used to communicate with other robots.
	/// @param scriptControl - reference to script execution control object.
	ScriptEngineWorker(trikControl::BrickInterface &brick
			, trikNetwork::MailboxInterface * const mailbox
			, ScriptExecutionControl &scriptControl
			);

	/// Create and initialize a new script engine.
	/// Can be safely called from other threads.
	/// @param supportThreads - true if created engine should support creation of threads.
	QScriptEngine *createScriptEngine(bool supportThreads = true);

	/// Copies given script engine creating a new one with the same context as existing one.
	/// Note that functions will not be copied to a new engine due to limitations of Qt Scripting engine,
	/// they need to be re-evaluated manually.
	/// Can be safely called from other threads (to some extent. Original engine shall not simultaneously evaluate).
	QScriptEngine *copyScriptEngine(const QScriptEngine * const original);

	/// Registers given C++ function as callable from script, with given name.
	/// Can be safely called from other threads (but it shall not be called simultaneously with engine creation).
	void registerUserFunction(const QString &name, QScriptEngine::FunctionSignature function);

	/// Helper for adding custom initialization steps when creating script engine from outside of the TrikRuntime.
	void addCustomEngineInitStep(const std::function<void (QScriptEngine *)> &step);

	/// Clears execution state and stops robot.
	/// Can be safely called from other threads.
	void resetBrick();

	/// Stops script execution and resets script engine. Can be called from another thread. By the end of call the
	/// worker would be in a ready state.
	/// Can be safely called from other threads.
	void stopScript();

signals:
	/// Emitted when current script execution is completed or is aborted by reset() call.
	/// @param error - localized error message or empty string.
	/// @param scriptId - unique identifier of a script completed
	void completed(const QString &error, int scriptId);

	/// Emitted when new script is started.
	/// @param scriptId - unique identifier assigned to a newly started script.
	void startedScript(int scriptId);

public slots:
	/// Starts script evaluation, emits startedScript() signal and returns. Script will be executed asynchronously.
	/// completed() signal is emitted upon script abortion or completion.
	/// It is a caller's responsibility to ensure that ScriptEngineWorker is in ready state before a call to run()
	/// by calling reset() first.
	/// @param script - QtScript code to evaluate
	/// @param scriptId - an id of a script, used to distinguish between different scripts run by a worker
	/// Can be safely called from other threads.
	void run(const QString &script, int scriptId);

	/// Runs a command in a `current` context. Permits to run a script line by line.
	/// The command will be executed asynchronously.
	/// If called when an ordinary script is running, that script would be aborted before evaluation of a command.
	/// In this mode, threads and messages are unsupported.
	/// Can be safely called from other threads.
	void runDirect(const QString &command, int scriptId);

	/// Plays "beep" sound.
	/// Can be safely called from other threads.
	void brickBeep();

private slots:
	/// Abort script execution.
	void onScriptRequestingToQuit();

	/// Actually runs given script. Is to be called from a thread owning ScriptEngineWorker.
	void doRun(const QString &script);

	/// Actually runs given command. Is to be called from a thread owning ScriptEngineWorker.
	void doRunDirect(const QString &command, int scriptId);

private:
	/// State of a script
	/// @value ready - worker is waiting for a new script
	/// @value starting - worker is preparing itself to running a script
	/// @value resetting - worker has finished execution of a script and returning to ready state
	/// @value running - worker is executing a script
	enum State {
		ready
		, starting
		, stopping
		, running
	};

	/// Turns the worker to a starting state, emits startedScript() signal.
	void startScriptEvaluation(int scriptId);

	/// Evaluates "system.js" file in given engine.
	void evalSystemJs(QScriptEngine * const engine) const;

	trikControl::BrickInterface &mBrick;
	trikNetwork::MailboxInterface * const mMailbox;  // Does not have ownership.
	ScriptExecutionControl &mScriptControl;
	Threading mThreading;
	QScriptEngine *mDirectScriptsEngine = nullptr;  // Has ownership.
	int mScriptId = 0;
	State mState = State::ready;
	QHash<QString, QScriptEngine::FunctionSignature> mRegisteredUserFunctions;
	QVector<std::function<void (QScriptEngine *)>> mCustomInitSteps;

	/// Ensures that there is only one instance of StopScript running at any given time, to prevent unpredicted
	/// behavior when programs are started and stopped actively.
	QMutex mScriptStateMutex;
};

}
