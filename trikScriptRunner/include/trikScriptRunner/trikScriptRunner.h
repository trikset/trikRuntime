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
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtScript/QScriptEngine>

namespace trikScriptRunner {

class ScriptEngineWorker;

/// Executes scripts in Qt Scripting Engine.
class TrikScriptRunner : public QObject
{
	Q_OBJECT

public:
	TrikScriptRunner();

	~TrikScriptRunner();

	/// Executes given script.
	/// @param script - script in Qt Script language to be executed.
	void run(QString const &script);

	/// Reads a script from given file and executes it.
	/// @param fileName - name of a file with script.
	/// @param synchronous - true, if this method shall not return until script execution is finished. Dangerous
	///   because scripts may contain infinite loops.
	void runFromFile(QString const &fileName, bool synchronous = false);

	/// Aborts script execution.
	void abort();

signals:
	/// Signal for script runner thread to begin execution.
	void threadRun(QString const &script);

	/// Signal for script runner thread to delete itself when possible.
	void threadDelete();

private:
	ScriptEngineWorker *mEngineWorker;  // Has ownership.
	QThread mWorkerThread;  // Has ownership.
};

}
