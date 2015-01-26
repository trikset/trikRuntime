#include "scriptThread.h"

#include "QsLog.h"

using namespace trikScriptRunner;

ScriptThread::ScriptThread(QString const &id, QScriptEngine *engine, QString const &script)
	: mId(id)
	, mEngine(engine)
	, mScript(script)
	, mFinished(false)
{
}

ScriptThread::~ScriptThread()
{
}

void ScriptThread::run()
{
	QLOG_INFO() << "ScriptThread: started thread" << this;

	mEngine->evaluate(mScript);
	if (mEngine->hasUncaughtException()) {
		int const line = mEngine->uncaughtExceptionLineNumber();
		QString const message = mEngine->uncaughtException().toString();
		mError = tr("Line %1: %2").arg(QString::number(line), message);
		qDebug() << "Uncaught exception at line" << line << ":" << message;
		QLOG_ERROR() << "Uncaught exception at line" << line << ":" << message;
	}

	mEngine->deleteLater();
	mFinished = true;
	QLOG_INFO() << "ScriptThread: ended evaluation, thread" << this;
}

void ScriptThread::abort()
{
	mEngine->abortEvaluation();
	while (!mFinished) {
		msleep(100);
	}
}

QString ScriptThread::id() const
{
	return mId;
}

QScriptEngine * ScriptThread::engine() const
{
	return mEngine;
}

QString ScriptThread::error() const
{
	return mError;
}
