#pragma once

#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>

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

	Q_INVOKABLE void sendMessage(const QString &threadId, const QScriptValue &message);

	Q_INVOKABLE QScriptValue receiveMessage();

	void waitForAll();

	void reset();

	QString errorMessage() const;

	void threadFinished(const QString &id);

private:
	void startThread(QString const &threadId, QScriptEngine *engine, QString const &script);
	QScriptEngine *cloneEngine(QScriptEngine *engine);
	bool tryLockReset();

	QHash<QString, ScriptThread *> mThreads;
	QMutex mThreadsMutex;
	QString mErrorMessage;

	QHash<QString, QQueue<QScriptValue> > mMessageQueues;
	QMutex mMessageMutex;
	QHash<QString, QMutex *> mMessageQueueMutexes;
	QHash<QString, QWaitCondition *> mMessageQueueConditions;

	bool mResetStarted;
	QMutex mResetMutex;

	ScriptEngineWorker *mScriptWorker;
	QString mScript;
};

}
