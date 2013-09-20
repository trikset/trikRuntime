/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
