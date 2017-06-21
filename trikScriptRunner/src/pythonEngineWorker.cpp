#include <QsLog.h>

#include <trikNetwork/mailboxInterface.h>
#include <trikKernel/paths.h>

#include "pythonEngineWorker.h"

using namespace trikScriptRunner;

PythonEngineWorker::PythonEngineWorker(trikControl::BrickInterface &brick
		, trikNetwork::MailboxInterface * const mailbox
		)
	: mBrick(brick)
	, mMailbox(mailbox)
	, mState(ready)
{}

void PythonEngineWorker::initPythonQt()
{
	PythonQt::init(PythonQt::IgnoreSiteModule);
	mMainContext = PythonQt::self()->getMainModule();

	initTrik();
}

void PythonEngineWorker::recreateContext()
{
	/// Delete current __main__ and create a new one
	mMainContext.evalScript("import sys");
	mMainContext.evalScript("del sys.modules[__name__]");
	mMainContext = PythonQt::self()->createModuleFromScript("__main__", "");

	initTrik();
}

void PythonEngineWorker::initTrik()
{
	PythonQt_init_PyTrikControl(mMainContext);
	mMainContext.addObject("brick", &mBrick);
}

void PythonEngineWorker::resetBrick()
{
	QLOG_INFO() << "Stopping robot";

	if (mMailbox) {
		mMailbox->stopWaiting();
		mMailbox->clearQueue();
	}

	mBrick.reset();
}

void PythonEngineWorker::brickBeep()
{
	mBrick.playSound(trikKernel::Paths::mediaPath() + "media/beep_soft.wav");
}

void PythonEngineWorker::stopScript()
{
	QMutexLocker locker(&mScriptStateMutex);

	if (mState == stopping) {
		// Already stopping, so we can do nothing.
		return;
	}

	if (mState == ready) {
		// Engine is ready for execution.
		return;
	}

	while (mState == starting) {
		// Some script is starting right now, so we are in inconsistent state. Let it start, then stop it.
		QThread::yieldCurrentThread();
	}

	QLOG_INFO() << "PythonEngineWorker: stopping script";

	mState = stopping;

	if (mMailbox) {
		mMailbox->stopWaiting();
	}

	QMetaObject::invokeMethod(this, "recreateContext"); /// recreates python module, which we use

	mState = ready;

	/// @todo: is it actually stopped?

	QLOG_INFO() << "PythonEngineWorker: stopping complete";
}

void PythonEngineWorker::run(const QString &script)
{
	QMutexLocker locker(&mScriptStateMutex);
	mState = starting;
	emit startedScript(0);
	QMetaObject::invokeMethod(this, "doRun", Q_ARG(const QString &, script));
}

void PythonEngineWorker::doRun(const QString &script)
{
	/// When starting script execution (by any means), clear button states.
	mBrick.keys()->reset();

	mMainContext.evalScript(script);

	mState = running;
	QLOG_INFO() << "PythonEngineWorker: evaluation ended";
	emit completed("", 0);
}

void PythonEngineWorker::runDirect(const QString &command)
{
	qDebug() << "PythonEngineWorker::runDirect";

	QMutexLocker locker(&mScriptStateMutex);
	QMetaObject::invokeMethod(this, "doRunDirect", Q_ARG(const QString &, command));
}

void PythonEngineWorker::doRunDirect(const QString &command)
{
	if (PythonQt::self()->hadError()) {
		PythonQt::self()->clearError();
		recreateContext();
	}
	mMainContext.evalScript(command);
}

void PythonEngineWorker::onScriptRequestingToQuit()
{
	throw "Not implemented";
}
