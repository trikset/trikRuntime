/* Copyright 2014 CyberTech Labs Ltd.
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

#include <QtCore/QQueue>
#include <QtCore/QThread>
#include <QtCore/QMutex>
#include <QtCore/QWaitCondition>
#include <QtCore/QSet>

#include <QtScript/QScriptEngine>

#include "scriptExecutionControl.h"

namespace trikScriptRunner {

class ScriptEngineWorker;
class ScriptThread;

/// Designed to support OS threads from a Qt Script.
/// Provides methods for creation and joining threads and for sending messages between threads.
class TRIKSCRIPTRUNNER_EXPORT Threading : public QObject
{
	Q_OBJECT

public:
	/// Constructs a Threading object with given script worker as a parent.
	explicit Threading(ScriptEngineWorker *scriptWorker, TrikScriptControlInterface &scriptControl);
	~Threading() override;

	/// Starts the main thread of a script
	void startMainThread(const QString &script);

	/// Starts a thread with given threadId.
	/// @param function - a thread routine
	Q_INVOKABLE void startThread(const QScriptValue &threadId, const QScriptValue &function);

	/// Joins a thread with given threadId. Does nothing if there is no thread with such id.
	Q_INVOKABLE void joinThread(const QString &threadId);

	/// Sends message to a mailbox with given threadId, even if such thread does not exist.
	/// The message can be accessed in the future by any thread with the same threadId.
	Q_INVOKABLE void sendMessage(const QString &threadId, const QScriptValue &message);

	/// Designed to be called from a thread receiving a message.
	Q_INVOKABLE QScriptValue receiveMessage(bool waitForMessage = true);

	/// Stops given thread.
	Q_INVOKABLE void killThread(const QString &threadId);

	/// Wait until all threads finish execution.
	/// During this function execution other events can be processed.
	void waitForAll();

	/// Aborts evalutation of all threads, resets to initial state.
	Q_INVOKABLE void reset();

	/// The last error message.
	QString errorMessage() const;

	/// Designed to be called from a thread that's finished execution.
	void threadFinished(const QString &id);

	/// Returns true if the script is being evaluated in event-driven mode.
	bool inEventDrivenMode() const;

signals:
	/// Signals that all threads have finished.
	void finished();

	/// Emitted when there is a request for variables values
	/// @param propertyName - name of variables prefix, i.e prefix "web" for variable "web.light"
	void getVariables(const QString &propertyName);

	/// Emitted when variables values are ready
	/// @param json - JSON container for variables values
	void variablesReady(const QJsonObject &data);

private:
	/// Starts a thread with given threadId
	/// @param engine - script engine that will do the work; it will be owned by a newly created thread
	/// @param script - exact script to evaluate in new thread
	void startThread(const QString &threadId, QScriptEngine *engine, const QString &script);

	/// Create new engine and initialize it with a context of given engine
	/// The caller is responsible for deletion of created engine.
	QScriptEngine *cloneEngine(QScriptEngine *engine);

	/// Utility function which locks reset mutex in case if reset is not started.
	bool tryLockReset();

	QHash<QString, QSharedPointer<ScriptThread>> mThreads;
	QSet<QString> mFinishedThreads;
	QSet<QString> mPreventFromStart;
	QMutex mThreadsMutex;
	QString mErrorMessage;

	QHash<QString, QQueue<QScriptValue>> mMessageQueues;
	QMutex mMessageMutex;
	QHash<QString, QMutex *> mMessageQueueMutexes;
	QHash<QString, QWaitCondition *> mMessageQueueConditions;

	bool mResetStarted = false;
	QMutex mResetMutex;

	ScriptEngineWorker * const mScriptWorker;  // Doesn't have ownership.
	TrikScriptControlInterface &mScriptControl;
	QString mScript;

	QScriptEngine *mMainScriptEngine; // Doesn't have ownership.
	const QString mMainThreadName = "main";
};

}
