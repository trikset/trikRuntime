#pragma once

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtScript/QScriptEngine>

namespace trikScriptRunner {

class ScriptEngineWorker;
class ScriptThread;

/// Provides methods for managing OS threads in QtScript.
class Threading : public QObject
{
	Q_OBJECT

public:
	explicit Threading(ScriptEngineWorker *scriptWorker);
	~Threading();

	void startMainThread(QString const &script);

	Q_INVOKABLE void startThread(QScriptValue const &threadId, QScriptValue const &function);

	Q_INVOKABLE void joinThread(QString const &threadId);

	void waitForAll();

	void reset();

	QString errorMessage() const;

private slots:
	void threadFinished();

private:
	void startThread(QString const &threadId, QScriptEngine *engine, QString const &script);
	QScriptEngine *cloneEngine(QScriptEngine *engine);

	QHash<QString, ScriptThread *> mThreads;
	QMutex mThreadsMutex;
	QString mErrorMessage;

	bool mResetStarted;
	QMutex mResetMutex;

	ScriptEngineWorker *mScriptWorker;
	QString mScript;
};

}
