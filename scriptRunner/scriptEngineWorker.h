#pragma once

#include <QtCore/QString>
#include <QtCore/QThread>
#include <QtScript/QScriptEngine>

#include <trikControl/brick.h>

namespace scriptRunner
{

/// Worker object to be run in a separate thread for Qt Script execution. QScriptEngine calls ProcessEvents too
/// infrequently even when ProcessEventsInterval is set to 1 ms, so there is a need for separate threads to
/// run a script and listen for incoming connections.
class ScriptEngineWorker : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	ScriptEngineWorker();

	/// Returns true, if script engine is initialized and running.
	bool isRunning() const;

	/// Aborts script execution.
	void abort();

public slots:
	/// Executes given script.
	void run(QString const &script);

	/// Marks worker for deletion and stops worker thread.
	void deleteWorker();

private:
	QScriptEngine mEngine;
	trikControl::Brick mBrick;
};

}
