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

#include "include/trikScriptRunner/trikScriptRunner.h"

#include <trikKernel/fileUtils.h>
#include <trikKernel/paths.h>

#include "src/scriptEngineWorker.h"
#include "src/scriptExecutionControl.h"

#include <QsLog.h>

using namespace trikScriptRunner;

TrikScriptRunner::TrikScriptRunner(trikControl::BrickInterface &brick
		, trikNetwork::MailboxInterface * const mailbox
		)
	: mScriptController(new ScriptExecutionControl())
	, mScriptEngineWorker(new ScriptEngineWorker(brick, mailbox, *mScriptController))
	, mMaxScriptId(0)
{
	connect(&mWorkerThread, SIGNAL(finished()), mScriptEngineWorker, SLOT(deleteLater()));
	connect(&mWorkerThread, SIGNAL(finished()), &mWorkerThread, SLOT(deleteLater()));
	if (mailbox) {
		connect(mailbox, SIGNAL(newMessage(int, QString)), this, SLOT(sendMessageFromMailBox(int, QString)));
	}

	mScriptEngineWorker->moveToThread(&mWorkerThread);

	connect(mScriptEngineWorker, SIGNAL(completed(QString, int)), this, SIGNAL(completed(QString, int)));
	connect(mScriptEngineWorker, SIGNAL(startedScript(int)), this, SLOT(onScriptStart(int)));

	connect(mScriptController.data(), SIGNAL(sendMessage(QString)), this, SIGNAL(sendMessage(QString)));

	QLOG_INFO() << "Starting TrikScriptRunner worker thread" << &mWorkerThread;

	mWorkerThread.start();
}

TrikScriptRunner::~TrikScriptRunner()
{
	mScriptEngineWorker->stopScript();
	QMetaObject::invokeMethod(&mWorkerThread, "quit");
	mWorkerThread.wait(1000);
}

void TrikScriptRunner::registerUserFunction(const QString &name, QScriptEngine::FunctionSignature function)
{
	mScriptEngineWorker->registerUserFunction(name, function);
}

void TrikScriptRunner::addCustomEngineInitStep(const std::function<void (QScriptEngine *)> &step)
{
	mScriptEngineWorker->addCustomEngineInitStep(step);
}

void TrikScriptRunner::brickBeep()
{
	QMetaObject::invokeMethod(mScriptEngineWorker, "brickBeep");
}

void TrikScriptRunner::run(const QString &script, const QString &fileName)
{
	const int scriptId = mMaxScriptId++;
	QLOG_INFO() << "TrikScriptRunner: new script" << scriptId << "from file" << fileName;
	mScriptEngineWorker->stopScript();

	if (!fileName.isEmpty()) {
		mScriptFileNames[scriptId] = fileName;
	}

	mScriptEngineWorker->run(script, (fileName.isEmpty() ? -1 : scriptId));
}

void TrikScriptRunner::runDirectCommand(const QString &command)
{
	QLOG_INFO() << "TrikScriptRunner: new direct command" << command;
	mScriptEngineWorker->runDirect(command, mMaxScriptId++);
}

void TrikScriptRunner::abort()
{
	mScriptEngineWorker->stopScript();
	mScriptEngineWorker->resetBrick();
}

void TrikScriptRunner::onScriptStart(int scriptId)
{
	if (scriptId != -1 && mScriptFileNames.contains(scriptId)) {
		emit startedScript(mScriptFileNames[scriptId], scriptId);
	} else {
		emit startedDirectScript(scriptId);
	}
}

void TrikScriptRunner::sendMessageFromMailBox(int senderNumber, const QString &message)
{
	emit sendMessage(QString("mail: sender: %1 contents: %2")
			.arg(senderNumber)
			.arg(message)
	);
}
