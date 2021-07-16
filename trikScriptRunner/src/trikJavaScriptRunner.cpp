/* Copyright 2014 - 2015 CyberTech Labs Ltd.
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

#include "trikJavaScriptRunner.h"

#include <trikKernel/fileUtils.h>
#include <trikKernel/paths.h>

#include "src/scriptEngineWorker.h"
#include "src/scriptExecutionControl.h"

#include <QEventLoop>
#include <QsLog.h>

using namespace trikScriptRunner;

TrikJavaScriptRunner::TrikJavaScriptRunner(trikControl::BrickInterface *brick
										   , trikNetwork::MailboxInterface * mailbox
										   , QSharedPointer<TrikScriptControlInterface> scriptControl
										   )
	: mScriptController(scriptControl)
	, mScriptEngineWorker(new ScriptEngineWorker(brick, mailbox, mScriptController.data()))
	, mMaxScriptId(0)
	, mVariablesServer(new TrikVariablesServer())
{
	mScriptEngineWorker->moveToThread(&mWorkerThread);

	connect(&mWorkerThread, &QThread::finished, mScriptEngineWorker, &QThread::deleteLater);
	connect(mScriptEngineWorker, &ScriptEngineWorker::completed, this, &TrikJavaScriptRunner::completed);
	connect(mScriptEngineWorker, &ScriptEngineWorker::startedScript, this, &TrikJavaScriptRunner::onScriptStart);

	connect(mScriptController.data(), &TrikScriptControlInterface::textInStdOut
			, this, &TrikJavaScriptRunner::textInStdOut);

	connect(mVariablesServer.data(), &TrikVariablesServer::getVariables
		, mScriptEngineWorker, &ScriptEngineWorker::getVariables);
	connect(mScriptEngineWorker, &ScriptEngineWorker::variablesReady
		, mVariablesServer.data(), &TrikVariablesServer::sendHTTPResponse);

	QLOG_INFO() << "Starting TrikJavaScriptRunner worker thread" << &mWorkerThread;

	mWorkerThread.setObjectName(mScriptEngineWorker->metaObject()->className());
	mWorkerThread.start();
}

TrikJavaScriptRunner::~TrikJavaScriptRunner()
{
	QEventLoop wait;
	connect(&mWorkerThread, &QThread::finished, &wait, &QEventLoop::quit);
	mScriptEngineWorker->stopScript();
	mWorkerThread.quit();
	// We need an event loop to process pending calls from dying thread to the current
	// mWorkerThread.wait(); // <-- !!! blocks pending calls
	wait.exec();
}

void TrikJavaScriptRunner::registerUserFunction(const QString &name, QScriptEngine::FunctionSignature function)
{
	mScriptEngineWorker->registerUserFunction(name, function);
}

void TrikJavaScriptRunner::addCustomEngineInitStep(const std::function<void (QScriptEngine *)> &step)
{
	mScriptEngineWorker->addCustomEngineInitStep(step);
}

void TrikJavaScriptRunner::brickBeep()
{
	QMetaObject::invokeMethod(mScriptEngineWorker, &ScriptEngineWorker::brickBeep);
}

void TrikJavaScriptRunner::setWorkingDirectory(const QString &workingDir)
{
	QMetaObject::invokeMethod(mScriptEngineWorker, [this, workingDir]()
				{mScriptEngineWorker->setWorkingDir(workingDir);});
}

void TrikJavaScriptRunner::run(const QString &script, const QString &fileName)
{
	const int scriptId = mMaxScriptId++;
	QLOG_INFO() << "TrikJavaScriptRunner: new script" << scriptId << "from file" << fileName;
	mScriptEngineWorker->stopScript();

	if (!fileName.isEmpty()) {
		mScriptFileNames[scriptId] = fileName;
	}

	mScriptEngineWorker->run(script, (fileName.isEmpty() ? -1 : scriptId));
}

void TrikJavaScriptRunner::runDirectCommand(const QString &command)
{
	QLOG_INFO() << "TrikJavaScriptRunner: new direct command" << command;
	mScriptEngineWorker->runDirect(command, mMaxScriptId++);
}

void TrikJavaScriptRunner::abort()
{
	if (mScriptEngineWorker) {
		mScriptEngineWorker->stopScript();
		mScriptEngineWorker->resetBrick();
	}
}

void TrikJavaScriptRunner::onScriptStart(int scriptId)
{
	if (scriptId != -1 && mScriptFileNames.contains(scriptId)) {
		emit startedScript(mScriptFileNames[scriptId], scriptId);
	} else {
		emit startedDirectScript(scriptId);
	}
}

QStringList TrikJavaScriptRunner::knownMethodNames() const
{
	return mScriptEngineWorker->knownMethodNames();
}


