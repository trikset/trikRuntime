/* Copyright 2014 CyberTech Labs Ltd.
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

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>

#include <trikControl/brickInterface.h>
#include <trikNetwork/mailboxInterface.h>
#include <trikNetwork/gamepadInterface.h>

namespace trikScriptRunner {

class ScriptExecutionControl;
class ScriptRunnerProxy;

/// Executes scripts in Qt Scripting Engine.
class TrikScriptRunner : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param brick - reference to trikControl::Brick instance.
	/// @param startDirPath - path to the directory from which the application was executed.
	TrikScriptRunner(trikControl::BrickInterface &brick
			, trikNetwork::MailboxInterface &mailbox
			, trikNetwork::GamepadInterface &gamepad
			, QString const &startDirPath);

	~TrikScriptRunner() override;

	/// Returns path to the directory in which scripts must be saved
	QString scriptsDirPath() const;

	/// Returns name of the directory in which scripts must be saved
	QString scriptsDirName() const;

public slots:
	/// Executes given script asynchronously. If some script is already executing, it will be aborted (but no
	/// completed() signal will be sent for it). Execution state will be reset (and robot fully stopped) before and
	/// after script execution. For event-driven mode (where script has brick.run() command) script counts as finished
	/// when it requests to quit by itself or was aborted. When script is finished, completed() signal will be emitted.
	/// @param script - script in Qt Script language to be executed.
	/// @param fileName - name of a file from which the script was loaded.
	/// @warning: The multithreaded script must not contain useful actions in the global context
	/// (function calls, variable initializations and so on in the global context is restricted).
	/// The reason is in non-thread-safety of script engine. We must run scripts by separate script engines
	/// and thus to have an opportunity to start concrete function from the given file. But QScriptEngine API
	/// has no such possibility so we should append function call to the end of the script. So if script will
	/// run some actions in the global context they will be invoked on each thread start.
	void run(QString const &script, QString const &fileName);

	/// Executes given direct script asynchronously. Same as run() method above, but without fileName parameter, as
	/// direct scripts are not stored in files.
	void run(QString const &script);

	/// Executes given script as direct command, so it will use existing script execution environment (or create one
	/// if needed) and will not reset execution state before or after execution. Sequence of direct commands counts
	/// as finished when one of them directly requests to quit (by brick.quit() command), then robot will be stopped,
	/// execution state will reset and completed() signal will be emitted.
	/// @param command - script in Qt Script to be executed as direct command.
	void runDirectCommand(QString const &command);

	/// Aborts script execution. completed() signal will be emitted when script will be actually aborted, robot will
	/// be stopped and execution state will be reset. Note that direct commands and scripts in event-driven mode will
	/// be stopped as well.
	void abort();

	/// Plays "beep" sound.
	void brickBeep();

signals:
	/// Emitted when current script completes execution (for event-driven mode it means that script requested to quit
	/// or was aborted).
	/// @param error - localized error message if any error occured during script execution or empty string
	/// if everything is fine.
	/// @param scriptId - unique identifier of a script completed
	void completed(QString const &error, int scriptId);

	/// Emitted when new script from file started.
	/// @param fileName - name of a file from where the script was loaded.
	/// @param scriptId - unique id of executed script assigned when script started.
	void startedScript(QString const &fileName, int scriptId);

	/// Emitted when new direct script started.
	/// @param scriptId - unique id of executed script assigned when script started.
	void startedDirectScript(int scriptId);

private slots:
	void onScriptStart(int scriptId);

private:
	QScopedPointer<ScriptExecutionControl> mScriptController;

	/// Proxy for script engine thread.
	QScopedPointer<ScriptRunnerProxy> mScriptRunnerProxy;

	QString mStartDirPath;

	int mMaxScriptId;

	QHash<int, QString> mScriptFileNames;
};

}
