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

#include <QsLog.h>

using namespace trikScriptRunner;

TrikJavaScriptRunner::TrikJavaScriptRunner(trikControl::BrickInterface &brick
										   , trikNetwork::MailboxInterface * const mailbox
										   )
	: mScriptController(new ScriptExecutionControl(brick))
	, mScriptEngineWorker(new ScriptEngineWorker(brick, mailbox, *mScriptController))
	, mMaxScriptId(0)
	, mVariablesServer(new TrikVariablesServer())
{
	mScriptEngineWorker->moveToThread(&mWorkerThread);

	connect(&mWorkerThread, &QThread::finished, mScriptEngineWorker, &QThread::deleteLater);
	connect(mScriptEngineWorker, &ScriptEngineWorker::completed, this, &TrikJavaScriptRunner::completed);
	connect(mScriptEngineWorker, &ScriptEngineWorker::startedScript, this, &TrikJavaScriptRunner::onScriptStart);

	connect(&*mScriptController, &ScriptExecutionControl::textInStdOut, this, &TrikJavaScriptRunner::textInStdOut);

	connect(mVariablesServer.data(), &TrikVariablesServer::getVariables
		, mScriptEngineWorker, &ScriptEngineWorker::getVariables);
	connect(mScriptEngineWorker, &ScriptEngineWorker::variablesReady
		, mVariablesServer.data(), &TrikVariablesServer::sendHTTPResponse);

	QLOG_INFO() << "Starting TrikJavaScriptRunner worker thread" << &mWorkerThread;

	mWorkerThread.start();
}

TrikJavaScriptRunner::~TrikJavaScriptRunner()
{
	mScriptEngineWorker->stopScript();
	mWorkerThread.quit();
	mWorkerThread.wait();
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
	QMetaObject::invokeMethod(mScriptEngineWorker, "brickBeep");
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
	mScriptEngineWorker->stopScript();
	mScriptEngineWorker->resetBrick();
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


