/* Copyright 2018 Iakov Kirilenko, CyberTech Labs Ltd.
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
#include <QMutex>

#include <trikControl/brickInterface.h>
#include <trikNetwork/mailboxInterface.h>

#include "PythonQt_QtAll.h"
#include "PyTrikControl0.h"

void PythonQt_init_PyTrikControl(PyObject* module);

namespace trikScriptRunner
{

/// Worker object to be run in a separate thread for Python execution.
class PythonEngineWorker : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param brick - reference to trikControl::Brick instance.
	/// @param mailbox - mailbox object used to communicate with other robots.
	PythonEngineWorker(trikControl::BrickInterface &brick
			, trikNetwork::MailboxInterface * mailbox
			);

	~PythonEngineWorker();

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
	/// It is a caller's responsibility to ensure that PythonEngineWorker is in ready state before a call to run()
	/// by calling reset() first.
	/// @param script - QtScript code to evaluate
	/// Can be safely called from other threads.
	void run(const QString &script);

	/// Runs a command in a `current` context. Permits to run a script line by line.
	/// The command will be executed asynchronously.
	/// If called when an ordinary script is running, that script would be aborted before evaluation of a command.
	/// In this mode, threads and messages are unsupported.
	/// Can be safely called from other threads.
	void runDirect(const QString &command);

	/// Initializes PythonQt and creates new main module, which will be used by user
	/// Must be invoked (called by the same thread as `run` or `runDirect`)
	/// Calls initTrik()
	void init();

	/// Recreates Main Context made by init
	void recreateContext();

	/// Plays "beep" sound.
	/// Can be safely called from other threads.
	void brickBeep();

private slots:
	/// Abort script execution.
	void onScriptRequestingToQuit();

	/// Adds trik object to main Python context
	void initTrik();

	/// Actually runs given script. Is to be called from a thread owning PythonEngineWorker.
	void doRun(const QString &script);

	/// Actually runs given command. Is to be called from a thread owning PythonEngineWorker.
	void doRunDirect(const QString &command);

	/// Appends new info from python stderr to current error message
	void updateErrorMessage(const QString &err);

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

	/// Evaluates "system.py" file in the current context.
	void evalSystemPy();

	/// Turns the worker to a starting state, emits startedScript() signal.
	void startScriptEvaluation(int scriptId);

	trikControl::BrickInterface &mBrick;
	trikNetwork::MailboxInterface * const mMailbox;  // Does not have ownership.

	State mState = State::ready;

	/// Ensures that there is only one instance of StopScript running at any given time, to prevent unpredicted
	/// behavior when programs are started and stopped actively.
	QMutex mScriptStateMutex;

	PythonQtObjectPtr mMainContext;

	PyThreadState * mPyInterpreter { nullptr };

	QString mErrorMessage;
};

}
