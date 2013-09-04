#pragma once

#include <QtCore/QString>
#include <QtCore/QTimer>
#include <QtCore/QThread>
#include <QtScript/QScriptEngine>

#include <trikControl/brick.h>

#include "scriptEngineWorker.h"

namespace scriptRunner {

/// Executes scripts in Qt Scripting Engine.
class Runner : public QObject
{
	Q_OBJECT

public:
	Runner();

	~Runner();

	/// Executes given script.
	/// @param script Script in Qt Script language to be executed.
	void run(QString const &script);

	/// Aborts script execution.
	void abort();

signals:
	/// Signal for script runner thread to begin execution.
	void threadRun(QString const &script);

	/// Signal for script runner thread to delete itself when possible.
	void threadDelete();

private:
	ScriptEngineWorker mEngineWorker;  // Has ownership.
	QThread mRunnerThread;  // Has ownership.
};

}
