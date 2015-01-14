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
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtCore/QWaitCondition>
#include <QtScript/QScriptEngine>

#include <trikControl/brickInterface.h>
#include <trikNetwork/mailboxInterface.h>
#include <trikNetwork/gamepadInterface.h>

#include "scriptExecutionControl.h"

namespace trikScriptRunner {

class ScriptEngineWorker;

/// Executes scripts in Qt Scripting Engine.
class ScriptRunnerProxy : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param brick - reference to trikControl::Brick instance.
	/// @param script - reference to script execution control object.
	/// @param startDirPath - path to the directory from which the application was executed.
	ScriptRunnerProxy(trikControl::BrickInterface &brick
			, trikNetwork::MailboxInterface * const mailbox
			, trikNetwork::GamepadInterface * const gamepad
			, ScriptExecutionControl &scriptControl
			, QString const &startDirPath);

	~ScriptRunnerProxy() override;

	/// Executes given script asynchronously. If some script is already executing, it will be aborted.
	/// @param script - script in Qt Script language to be executed.
	/// @param inEventDrivenMode - true, if script shall be launched in event driven mode, i.e. not be stopped after
	///        it finishes. In this mode event handlers in a script may be called until script is not explicitly
	///        stopped.
	/// @param function - the name of the function execution must start with. If empty then the script will be
	///        evaluated as-is, else function call will be appended to @arg script.
	void run(QString const &script, bool inEventDrivenMode, int scriptId, QString const &function = "main");

	/// Aborts script execution.
	void reset();

	/// Plays "beep" sound.
	void brickBeep();

signals:
	/// Emitted when current script completes execution.
	/// @param error - localized error message or empty string.
	/// @param scriptId - unique identifier of a script completed
	void completed(QString const &error, int scriptId);

	/// Emitted when script started execution.
	/// @param scriptId - identifier of a script started.
	void startedScript(int scriptId);

private:
	ScriptEngineWorker *mEngineWorker;  // Has ownership.
	QThread mWorkerThread;
};

}
