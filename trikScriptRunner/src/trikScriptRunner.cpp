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

#include "src/scriptEngineWorker.h"
#include "src/scriptExecutionControl.h"

#include <QsLog.h>

using namespace trikScriptRunner;

// name of the directory in which scripts must be saved
const QString constScriptsDirName = "scripts";

TrikScriptRunner::TrikScriptRunner(trikControl::BrickInterface &brick
		, trikNetwork::MailboxInterface * const mailbox
		, trikNetwork::GamepadInterface * const gamepad
		, const QString &startDirPath)
	: mScriptController(new ScriptExecutionControl())
	, mScriptEngineWorker(new ScriptEngineWorker(brick, mailbox, gamepad, *mScriptController, startDirPath))
	, mStartDirPath(startDirPath)
	, mMaxScriptId(0)
{
	connect(&mWorkerThread, SIGNAL(finished()), mScriptEngineWorker, SLOT(deleteLater()));
	connect(&mWorkerThread, SIGNAL(finished()), &mWorkerThread, SLOT(deleteLater()));

	mScriptEngineWorker->moveToThread(&mWorkerThread);

	connect(mScriptEngineWorker, SIGNAL(completed(QString, int)), this, SIGNAL(completed(QString, int)));
	connect(mScriptEngineWorker, SIGNAL(startedScript(int)), this, SLOT(onScriptStart(int)));

	connect(mScriptController.data(), SIGNAL(sendMessage(QString)), this, SIGNAL(sendMessage(QString)));

	mWorkerThread.start();
}

TrikScriptRunner::~TrikScriptRunner()
{
	mScriptEngineWorker->reset();
	QMetaObject::invokeMethod(&mWorkerThread, "quit");
	mWorkerThread.wait(1000);
}

QString TrikScriptRunner::scriptsDirPath() const
{
	return QString(mStartDirPath + constScriptsDirName);
}

QString TrikScriptRunner::scriptsDirName() const
{
	return constScriptsDirName;
}

void TrikScriptRunner::brickBeep()
{
	QMetaObject::invokeMethod(mScriptEngineWorker, "brickBeep");
}

void TrikScriptRunner::run(const QString &script, const QString &fileName)
{
	const int scriptId = mMaxScriptId++;
	QLOG_INFO() << "TrikScriptRunner: new script" << scriptId << "from file" << fileName;
	mScriptEngineWorker->reset();

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
	mScriptEngineWorker->reset();
}

void TrikScriptRunner::onScriptStart(int scriptId)
{
	if (scriptId == -1) {
		return;
	}

	if (mScriptFileNames.contains(scriptId)) {
		emit startedScript(mScriptFileNames[scriptId], scriptId);
	} else {
		emit startedDirectScript(scriptId);
	}
}
