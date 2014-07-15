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

namespace trikScriptRunner {
class ScriptRunnerProxy;
}

namespace trikCommunicator {

/// Represents script runner that is used by all connections and means to access it from other threads.
class ScriptRunnerWrapper : public QObject {
	Q_OBJECT

public:
	/// Constructor that creates its own instance of a script runner.
	/// @param configPath - path to config file for trikControl, for example, /home/root/trik/.
	/// @param startDirPath - path to the directory from which the application was executed.
	explicit ScriptRunnerWrapper(QString const &configFilePath, QString const &startDirPath);

	/// Constructor that accepts external script runner and issues commands to it.
	explicit ScriptRunnerWrapper(trikScriptRunner::ScriptRunnerProxy &runner);

	~ScriptRunnerWrapper();

signals:
	/// Emitted when script finishes execution and runner returns to "idle" state.
	void finishedScript();

public slots:
	/// Execute given script. Note that script is executed in another thread by script engine, so it queues a script
	/// for execution and returns immediately.
	void run(QString const &script);

	/// Stops current script execution. Also stops execution of all queued scripts.
	void stop();

private slots:
	/// Called when script runner reports that it has finished execution. Used to stop robot.
	void onScriptExecutionCompleted();

private:
	enum State {
		idle
		, running
		, stopping
	};

	/// Script runner object.
	trikScriptRunner::ScriptRunnerProxy *mRunner;  // Has or doesn't have ownership, depending on mOwnsRunner.

	/// Flag that is true when wrapper shall delete runner itself.
	bool const mOwnsRunner;

	/// Execution state of a runner.
	State mExecutionState;
};

}
