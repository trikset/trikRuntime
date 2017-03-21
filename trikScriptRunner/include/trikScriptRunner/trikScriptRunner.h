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

#include <functional>

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>
#include <QtCore/QThread>
#include <QtScript/QScriptEngine>

namespace trikNetwork {
class MailboxInterface;
}

namespace trikControl {
class BrickInterface;
}

namespace trikScriptRunner {

class ScriptEngineWorker;
class ScriptExecutionControl;

/// Executes scripts in Qt Scripting Engine.
class TrikScriptRunner : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param brick - reference to trikControl::Brick instance.
	/// @param mailbox - mailbox object used to communicate with other robots.
	TrikScriptRunner(trikControl::BrickInterface &brick
			, trikNetwork::MailboxInterface * const mailbox
			);

	~TrikScriptRunner() override;

	/// Registers given C++ function as callable from script, with given name.
	void registerUserFunction(const QString &name, QScriptEngine::FunctionSignature function);

	/// Adds custom initialization steps when creating script engine (useful when used from outside of the TrikRuntime).
	void addCustomEngineInitStep(const std::function<void (QScriptEngine *)> &step);

public slots:
	/// Executes given script asynchronously. If some script is already executing, it will be aborted.
	/// For event-driven mode (where script has brick.run() command) script counts as finished
	/// when it requests to quit by itself or was aborted. When script is finished, completed() signal will be emitted.
	/// @param script - script in Qt Script language to be executed.
	/// @param fileName - name of a file from which the script was loaded.
	/// @warning: The multithreaded script must not contain useful actions in the global context
	/// (function calls, variable initializations and so on in the global context is restricted).
	/// The reason is in non-thread-safety of script engine. We must run scripts by separate script engines
	/// and thus to have an opportunity to start concrete function from the given file. But QScriptEngine API
	/// has no such possibility so we should append function call to the end of the script. So if script will
	/// run some actions in the global context they will be invoked on each thread start.
	void run(const QString &script, const QString &fileName = "");

	/// Executes given script as direct command, so it will use existing script execution environment (or create one
	/// if needed) and will not reset execution state before or after execution. Sequence of direct commands counts
	/// as finished when one of them directly requests to quit (by brick.quit() command), then robot will be stopped,
	/// execution state will reset and completed() signal will be emitted.
	/// @param command - script in Qt Script to be executed as direct command.
	void runDirectCommand(const QString &command);

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
	void completed(const QString &error, int scriptId);

	/// Emitted when new script from file started.
	/// @param fileName - name of a file from where the script was loaded.
	/// @param scriptId - unique id of executed script assigned when script started.
	void startedScript(const QString &fileName, int scriptId);

	/// Emitted when new direct script started.
	/// @param scriptId - unique id of executed script assigned when script started.
	void startedDirectScript(int scriptId);

	/// Emitted when a message must be sent to a desktop.
	void sendMessage(const QString &text);

private slots:
	void onScriptStart(int scriptId);

	/// Sends message to host machine from mailbox via wifi.
	void sendMessageFromMailBox(int senderNumber, const QString &message);

private:
	QScopedPointer<ScriptExecutionControl> mScriptController;

	/// Has ownership, memory is managed by thread and deleteLater().
	ScriptEngineWorker *mScriptEngineWorker;
	QThread mWorkerThread;

	int mMaxScriptId;

	QHash<int, QString> mScriptFileNames;
};

}
