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

#include "threading.h"

#include <QtCore/QEventLoop>
#include <QtScript/QScriptValueIterator>
#include <QJsonObject>

#include "scriptEngineWorker.h"
#include "src/utils.h"
#include "src/scriptThread.h"

#include <QsLog.h>

using namespace trikScriptRunner;

Threading::Threading(ScriptEngineWorker *scriptWorker, TrikScriptControlInterface &scriptControl)
	: QObject(scriptWorker)
	, mScriptWorker(scriptWorker)
	, mScriptControl(scriptControl)
	, mMainScriptEngine(nullptr)
{
}

Threading::~Threading()
{
	reset();
}

void Threading::startMainThread(const QString &script)
{
	mScript = script;
	mErrorMessage.clear();
	mFinishedThreads.clear();
	mPreventFromStart.clear();

	const QRegExp mainRegexp(R"#((.*var main\s*=\s*\w*\s*function\(.*\).*)|(.*function\s+%1\s*\(.*\).*))#");
	const bool needCallMain = mainRegexp.exactMatch(script) && !script.trimmed().endsWith("main();");

	mMainScriptEngine = mScriptWorker->createScriptEngine();
	startThread(mMainThreadName, mMainScriptEngine, needCallMain ? script + "\nmain();" : script);
}

void Threading::startThread(const QScriptValue &threadId, const QScriptValue &function)
{
	startThread(threadId.toString(), cloneEngine(function.engine()), mScript + "\n" + function.toString() + "();");
}

void Threading::startThread(const QString &threadId, QScriptEngine *engine, const QString &script)
{
	QMutexLocker resetMutexLocker(&mResetMutex);

	if (mResetStarted) {
		QLOG_INFO() << "Threading: can't start new thread" << threadId << "with engine" << engine << "due to reset";
		delete engine;
		return;
	}

	QMutexLocker threadsMutexLocker(&mThreadsMutex);
	if (mThreads.contains(threadId)) {
		QLOG_ERROR() << "Threading: attempt to create a thread with an already occupied id" << threadId;
		mErrorMessage = tr("Attempt to create a thread with an already occupied id %1").arg(threadId);
		mThreads[threadId]->abort();
		return;
	}

	if (mPreventFromStart.contains(threadId)) {
		QLOG_INFO() << "Threading: attempt to create a thread which must be killed" << threadId;
		mPreventFromStart.remove(threadId);
		mFinishedThreads.insert(threadId);
		return;
	}

	QLOG_INFO() << "Starting new thread" << threadId << "with engine" << engine;
	auto thread = new ScriptThread(*this, threadId, engine, script);
	engine->moveToThread(thread);

	connect(thread, &QThread::finished, this, [this, threadId](){ threadFinished(threadId); });
	connect(&mScriptControl, &TrikScriptControlInterface::quitSignal, thread
			, &ScriptThread::stopRunning, Qt::DirectConnection);
	if (threadId == mMainThreadName) {
		connect(this, &Threading::getVariables, thread, &ScriptThread::onGetVariables);
		connect(thread, &ScriptThread::variablesReady, this, &Threading::variablesReady);
	}

	mThreads[threadId].reset(thread);
	mFinishedThreads.remove(threadId);

	QEventLoop wait;
	connect(thread, &QThread::started, &wait, &QEventLoop::quit);

	thread->setObjectName(engine->metaObject()->className());
	thread->start();
	wait.exec();
	QLOG_INFO() << "Threading: started thread" << threadId << "with engine" << engine << ", thread object" << thread;
}

void Threading::waitForAll()
{
	QEventLoop wait;
	connect(this, &Threading::finished, &wait, &QEventLoop::quit);
	mThreadsMutex.lock();
	auto hasThreads = !mThreads.isEmpty();
	mThreadsMutex.unlock();
	if (hasThreads) {
		wait.exec();
	}
}

void Threading::joinThread(const QString &threadId)
{
	QMutexLocker threadsMutexLocker(&mThreadsMutex);

	while ((!mThreads.contains(threadId) || !mThreads[threadId]->isRunning())
			&& !mFinishedThreads.contains(threadId))
	{
		if (mResetStarted) {
			return;
		}
		threadsMutexLocker.unlock();
		QThread::yieldCurrentThread();
		threadsMutexLocker.relock();
	}

	if (mFinishedThreads.contains(threadId)) {
		return;
	}

	mThreads[threadId]->wait();
}

QScriptEngine * Threading::cloneEngine(QScriptEngine *engine)
{
	QScriptEngine *result = mScriptWorker->copyScriptEngine(engine);
	result->evaluate(mScript);
	return result;
}

void Threading::reset()
{
	if (!tryLockReset()) {
		return;
	}

	mResetStarted = true;
	mResetMutex.unlock();
	QLOG_INFO() << "Threading: reset started";

	mMessageMutex.lock();
	for (auto &&condition : mMessageQueueConditions) {
		condition->wakeAll();
	}

	mMessageMutex.unlock();
	mThreadsMutex.lock();

	for (auto &&thread : mThreads) {
		mScriptControl.reset();  // TODO: find more sophisticated solution to prevent waiting after abortion
		thread->abort();
	}

	mFinishedThreads.clear();
	mThreadsMutex.unlock();
	mScriptControl.reset();

	waitForAll();

	qDeleteAll(mMessageQueueMutexes);
	qDeleteAll(mMessageQueueConditions);
	mMessageQueueMutexes.clear();
	mMessageQueueConditions.clear();
	mMessageQueues.clear();

	QLOG_INFO() << "Threading: reset ended";
	mResetStarted = false;
}

void Threading::threadFinished(const QString &id)
{
	QLOG_INFO() << "Finishing thread" << id;

	mResetMutex.lock();
	mThreadsMutex.lock();
	if (!mThreads[id]->error().isEmpty() && mErrorMessage.isEmpty()) {
		mErrorMessage = mThreads[id]->error();
	}

	QLOG_INFO() << "Thread" << id << "has finished, thread object" << mThreads[id];
	mThreads.remove(id);
	mFinishedThreads.insert(id);
	mThreadsMutex.unlock();
	mResetMutex.unlock();

	if (!mErrorMessage.isEmpty()) {
		reset();
	}

	if (mThreads.isEmpty()) {
		emit finished();
	}
}

void Threading::sendMessage(const QString &threadId, const QScriptValue &message)
{
	if (!tryLockReset()) {
		return;
	}

	mMessageMutex.lock();
	if (!mMessageQueueConditions.contains(threadId)) {
		mMessageQueueMutexes[threadId] = new QMutex();
		mMessageQueueConditions[threadId] = new QWaitCondition();
	}

	mMessageQueues[threadId].enqueue(message);
	mMessageQueueConditions[threadId]->wakeOne();
	mMessageMutex.unlock();

	mResetMutex.unlock();
}

QScriptValue Threading::receiveMessage(bool waitForMessage)
{
	if (!tryLockReset()) {
		return QScriptValue();
	}

	QString threadId = qobject_cast<ScriptThread *>(QThread::currentThread())->id();
	mMessageMutex.lock();
	if (!mMessageQueueConditions.contains(threadId)) {
		mMessageQueueMutexes[threadId] = new QMutex();
		mMessageQueueConditions[threadId] = new QWaitCondition();
	}

	QMutex *mutex = mMessageQueueMutexes[threadId];
	QWaitCondition *condition = mMessageQueueConditions[threadId];
	QQueue<QScriptValue> &queue = mMessageQueues[threadId];
	mMessageMutex.unlock();

	mutex->lock();
	if (queue.isEmpty()) {
		mResetMutex.unlock();
		if (!waitForMessage) {
			mutex->unlock();
			return QScriptValue("");
		}

		condition->wait(mutex);
		if (!tryLockReset()) {
			mutex->unlock();
			return QScriptValue();
		}
	}

	mutex->unlock();
	QScriptValue result = queue.dequeue();
	mResetMutex.unlock();
	return result;
}

void Threading::killThread(const QString &threadId)
{
	if (!tryLockReset()) {
		return;
	}

	mThreadsMutex.lock();
	if (!mThreads.contains(threadId)) {
		if (!mFinishedThreads.contains(threadId)) {
			QLOG_INFO() << "Threading: killing thread that is not started yet," << threadId
					<< "will be prevented from running";
			mPreventFromStart.insert(threadId);
		} else {
			QLOG_INFO() << "Threading: killing already finished thread, ignoring";
		}
	} else {
		QLOG_INFO() << "Threading: killing thread" << threadId;
		mThreads[threadId]->abort();
	}

	mThreadsMutex.unlock();
	mResetMutex.unlock();
}

QString Threading::errorMessage() const
{
	return mErrorMessage;
}

bool Threading::tryLockReset()
{
	mResetMutex.lock();
	if (mResetStarted) {
		mResetMutex.unlock();
	}

	return !mResetStarted;
}

bool Threading::inEventDrivenMode() const
{
	return mScriptControl.isInEventDrivenMode();
}
