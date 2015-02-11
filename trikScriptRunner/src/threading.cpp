#include "threading.h"

#include "scriptEngineWorker.h"
#include "src/utils.h"
#include "src/scriptThread.h"

#include "QsLog.h"

using namespace trikScriptRunner;

Threading::Threading(ScriptEngineWorker *scriptWorker)
	: QObject(scriptWorker)
	, mResetStarted(false)
	, mScriptWorker(scriptWorker)
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

	QRegExp const mainRegexp("(.*var main\\s*=\\s*\\w*\\s*function\\(.*\\).*)|(.*function\\s+%1\\s*\\(.*\\).*)");
	bool needCallMain = mainRegexp.exactMatch(script) && !script.trimmed().endsWith("main();");

	startThread("main", mScriptWorker->createScriptEngine(), needCallMain ? script + "\nmain();" : script);
}

void Threading::startThread(QScriptValue const &threadId, QScriptValue const &function)
{
	startThread(threadId.toString(), cloneEngine(function.engine()), mScript + "\n" + function.toString() + "();");
}

void Threading::startThread(QString const &threadId, QScriptEngine *engine, QString const &script)
{
	mResetMutex.lock();
	if (mResetStarted) {
		QLOG_INFO() << "Threading: can't start new thread" << threadId << "with engine" << engine << "due to reset";
		delete engine;
		mResetMutex.unlock();
		return;
	}

	QLOG_INFO() << "Threading: starting new thread" << threadId << "with engine" << engine;
	ScriptThread *thread = new ScriptThread(*this, threadId, engine, script);
	mThreadsMutex.lock();
	mThreads[threadId] = thread;
	mThreadsMutex.unlock();
	engine->moveToThread(thread);

	connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));

	thread->start();
	while (!thread->isEvaluating()) {
		QThread::yieldCurrentThread();
	}

	// wait until script actually start
	for (int i = 0; i < 500; ++i) {
		QThread::yieldCurrentThread();
	}

	QLOG_INFO() << "Threading: started thread" << threadId << "with engine" << engine << ", thread object" << thread;
	mResetMutex.unlock();
}

void Threading::waitForAll()
{
	while (!mThreads.isEmpty()) {
		QThread::yieldCurrentThread();
	}
}

void Threading::joinThread(const QString &threadId)
{
	mThreadsMutex.lock();
	if (!mThreads.contains(threadId)) {
		mThreadsMutex.unlock();
		return;
	}

	ScriptThread *thread = mThreads[threadId];
	mThreadsMutex.unlock();
	thread->wait();
}

QScriptEngine * Threading::cloneEngine(QScriptEngine *engine)
{
	QScriptEngine *result = mScriptWorker->createScriptEngine();
	result->evaluate(mScript);

	QScriptValue globalObject = result->globalObject();
	Utils::copyRecursivelyTo(engine->globalObject(), globalObject, result);
	result->setGlobalObject(globalObject);

	return result;
}

void Threading::reset()
{
	mResetMutex.lock();
	if (mResetStarted) {
		mResetMutex.unlock();
		return;
	}

	mResetStarted = true;
	mResetMutex.unlock();
	QLOG_INFO() << "Threading: reset started";

	mThreadsMutex.lock();
	for (ScriptThread *thread : mThreads.values()) {
		thread->abort();
	}

	mThreadsMutex.unlock();
	waitForAll();

	QLOG_INFO() << "Threading: reset ended";
	mResetStarted = false;
}

void Threading::threadFinished(const QString &id)
{
	QLOG_INFO() << "Threading: finishing thread" <<	id;

	mThreadsMutex.lock();
	if (!mThreads[id]->error().isEmpty() && mErrorMessage.isEmpty()) {
		mErrorMessage = mThreads[id]->error();
	}

	QLOG_INFO() << "Threading: thread" << id << "has finished, thread object" << mThreads[id];
	mThreads.remove(id);
	mThreadsMutex.unlock();

	if (!mErrorMessage.isEmpty()) {
		reset();
	}
}

QString Threading::errorMessage() const
{
	return mErrorMessage;
}
