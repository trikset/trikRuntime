#include "include/trikScriptRunner/trikScriptRunner.h"

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

#include "scriptableParts.h"
#include "scriptEngineWorker.h"

using namespace trikScriptRunner;

TrikScriptRunner::TrikScriptRunner()
{
	mEngineWorker = new ScriptEngineWorker();

	mEngineWorker->moveToThread(&mWorkerThread);

	connect(this, SIGNAL(threadRun(QString const &)), mEngineWorker, SLOT(run(QString const &)));
	connect(this, SIGNAL(threadDelete()), mEngineWorker, SLOT(deleteWorker()));

	mWorkerThread.start();
}

TrikScriptRunner::~TrikScriptRunner()
{
	mEngineWorker->abort();
	emit threadDelete();
	mWorkerThread.wait(1000);
}

void TrikScriptRunner::run(QString const &script)
{
	emit threadRun(script);
}

void TrikScriptRunner::abort()
{
	mEngineWorker->abort();
}
