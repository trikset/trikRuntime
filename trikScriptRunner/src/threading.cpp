#include "threading.h"

#include "scriptEngineWorker.h"

using namespace trikScriptRunner;

int const maxThreadsCount = 10;

Threading::Threading(ScriptEngineWorker &runner)
	: mRunner(runner)
{
	QThreadPool::globalInstance()->setMaxThreadCount(maxThreadsCount);
}

void Threading::start(QScriptValue const &function)
{
	ScriptThread *thread = new ScriptThread(mScript, function.toString(), mRunner);
	thread->setAutoDelete(true);
	QThreadPool::globalInstance()->start(thread);
}

QScriptValue Threading::activeThreadCount() const
{
	return QScriptValue(QThreadPool::globalInstance()->activeThreadCount());
}

QScriptValue Threading::maxThreadCount() const
{
	return QScriptValue(QThreadPool::globalInstance()->maxThreadCount());
}

QScriptValue Threading::waitForDone(QScriptValue const &timeout)
{
	return QScriptValue(QThreadPool::globalInstance()->waitForDone(timeout.toInt32()));
}

void Threading::setCurrentScript(QString const &script)
{
	mScript = script;
}

void Threading::waitForAll()
{
	QThreadPool::globalInstance()->waitForDone();
}

Threading::ScriptThread::ScriptThread(QString const &mainScript, QString const &function, ScriptEngineWorker &runner)
	: mScript(mainScript)
	, mFunction(function)
	, mRunner(runner.clone())
{
}

void Threading::ScriptThread::run()
{
	mRunner.run(mScript, false, mFunction);
}
