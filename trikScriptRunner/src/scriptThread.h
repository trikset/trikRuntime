#pragma once

#include <QtCore/QThread>
#include <QtScript/QScriptEngine>

namespace trikScriptRunner {

class ScriptThread : public QThread
{
	Q_OBJECT
public:
	ScriptThread(QString const &id, QScriptEngine *engine, QString const &script);
	~ScriptThread();

	void abort();

	QString id() const;
	QScriptEngine *engine() const;
	QString error() const;

signals:
	void finished(QString const &id);

protected:
	void run();

private:
	QString mId;
	QScriptEngine *mEngine;
	QString mScript;

	QString mError;
	bool mFinished;
};

}
