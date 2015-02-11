#pragma once

#include <QtCore/QThread>
#include <QtScript/QScriptEngine>

namespace trikScriptRunner {

class Threading;

class ScriptThread : public QThread
{
	Q_OBJECT
public:
	ScriptThread(Threading &threading, QString const &id, QScriptEngine *engine, QString const &script);
	~ScriptThread();

	void abort();

	QString id() const;
	QScriptEngine *engine() const;
	QString error() const;

	bool isEvaluating() const;

protected:
	void run();

private:
	QString mId;
	QScriptEngine *mEngine;
	QString mScript;

	Threading &mThreading;

	QString mError;
};

}
