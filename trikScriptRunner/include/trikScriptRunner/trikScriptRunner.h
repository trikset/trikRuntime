#pragma once

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

#include <QtCore/QObject>
#include <QtCore/QScopedPointer>

namespace trikScriptRunner {

class ScriptRunnerProxy;

/// Executes scripts in Qt Scripting Engine.
class TrikScriptRunner : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param configFilePath - path to config file for trikControl, for example /home/root/trik/
	/// @param startDirPath - path to the directory from which the application was executed.
	TrikScriptRunner(QString const &configFilePath, QString const &startDirPath);

	~TrikScriptRunner();

public slots:
	/// Executes given script asynchronously. If some script is already executing, it will be aborted (but no
	/// completed() signal will be sent for it). Execution state will be reset (and robot fully stopped) before and
	/// after script execution. For event-driven mode (where script has brick.run() command) script counts as finished
	/// when it requests to quit by itself or was aborted. When script is finished, completed() signal will be emitted.
	/// @param script - script in Qt Script language to be executed.
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

signals:
	/// Emitted when current script completes execution (for event-driven mode it means that script requested to quit
	/// or was aborted).
	void completed();

private slots:
	/// Called when script runner reports that it has finished execution. Used to stop robot.
	void onScriptExecutionCompleted();

private:
	enum State {
		idle
		, running
		, stopping
	};

	/// Proxy for script engine thread.
	QScopedPointer<ScriptRunnerProxy> mScriptRunnerProxy;

	/// Execution state of a runner.
	State mExecutionState;
};

}
