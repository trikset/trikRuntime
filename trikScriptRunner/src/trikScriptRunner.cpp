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

#include "src/scriptExecutionControl.h"
#include "src/scriptRunnerProxy.h"

#include <QsLog.h>

using namespace trikScriptRunner;

// name of the directory in which scripts must be saved
QString const constScriptsDirName = "scripts";

TrikScriptRunner::TrikScriptRunner(trikControl::BrickInterface &brick, QString const &startDirPath)
	: mScriptController(new ScriptExecutionControl())
	, mScriptRunnerProxy(new ScriptRunnerProxy(brick, *mScriptController, startDirPath))
	, mStartDirPath(startDirPath)
	, mMaxScriptId(0)
{
	connect(mScriptRunnerProxy.data(), SIGNAL(completed(QString, int))
			, this, SIGNAL(completed(QString, int)));

	connect(mScriptRunnerProxy.data(), SIGNAL(startedScript(int))
			, this, SLOT(onScriptStart(int)));
}

TrikScriptRunner::~TrikScriptRunner()
{
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
	mScriptRunnerProxy->brickBeep();
}

void TrikScriptRunner::run(QString const &script, QString const &fileName)
{
	QLOG_INFO() << "TrikScriptRunner: new script" << mMaxScriptId << "from file" << fileName;
	mScriptRunnerProxy->run(script, false, mMaxScriptId);
	mScriptFileNames[mMaxScriptId++] = fileName;
}

void TrikScriptRunner::run(QString const &script)
{
	QLOG_INFO() << "TrikScriptRunner: new direct script \"" << script << "\"";
	mScriptRunnerProxy->run(script, false, -1);
}

void TrikScriptRunner::runDirectCommand(QString const &command)
{
	QLOG_INFO() << "TrikScriptRunner: new direct command" << command;
	mScriptRunnerProxy->run(command, true, mMaxScriptId++, QString());
}

void TrikScriptRunner::abort()
{
	mScriptRunnerProxy->reset();
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
