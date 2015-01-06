/* Copyright 2014 Dmitry Mordvinov, CyberTech Labs Ltd.
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

#include <QtCore/QThreadPool>
#include <QtScript/QScriptEngine>

namespace trikScriptRunner {

class ScriptEngineWorker;

/// Provides methods for managing OS threads in QtScript.
class Threading : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param runner - will be used for starting new script engines.
	explicit Threading(ScriptEngineWorker &runner);

	/// Starts new OS thread invoking the given function.
	/// @warning: as far as QScriptEngine is not thread-safe threads have some restrictions.
	/// One of them described in TrikScriptRunner::run() documentation. The other one is that
	/// scripts have no shared memory and though they will see the global memory state on the
	/// moment when they start, further modifications will not be obtained by new thread.
	/// There may be added some special thread communication functions for this problem solution.
	Q_INVOKABLE void start(QScriptValue const &function);

	/// Returns a number of threads that created by Threading object.
	Q_INVOKABLE QScriptValue activeThreadCount() const;

	/// Returns the maximum number of threads used by the Threading object.
	Q_INVOKABLE QScriptValue maxThreadCount() const;

	/// Waits up to @arg timeout milliseconds for all threads to exit and removes all threads from
	/// the thread pool. Returns true if all threads were removed; otherwise it returns false.
	/// If @arg timeout is -1, the timeout is ignored (waits for the last thread to exit).
	Q_INVOKABLE QScriptValue waitForDone(QScriptValue const &timeout);

	/// @param script - a text of the script that is evaluated at the moment. New threads will
	/// invoke functions declared in this script.
	void setCurrentScript(QString const &script);

	/// Blocks current thread untill all threads in thread pool finish their execution.
	void waitForAll();

private:
	/// Separate thread for script execution.
	class ScriptThread : public QRunnable
	{
	public:
		/// Constructor.
		ScriptThread(QString const &mainScript, QString const &function, ScriptEngineWorker &runner);

	private:
		void run() override;

		QString const mScript;
		QString const mFunction;
		ScriptEngineWorker &mRunner;
	};

	QString mScript;
	ScriptEngineWorker &mRunner;
};

}
