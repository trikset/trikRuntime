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

#include "include/trikScriptRunner/trikScriptRunner.h"

#include <trikKernel/fileUtils.h>
#include "src/scriptRunnerProxy.h"

using namespace trikScriptRunner;

TrikScriptRunner::TrikScriptRunner(QString const &configFilePath, QString const &startDirPath)
	: mScriptRunnerProxy(new ScriptRunnerProxy(configFilePath, startDirPath))
	, mExecutionState(idle)
{
	connect(mScriptRunnerProxy.data(), SIGNAL(completed()), this, SLOT(onScriptExecutionCompleted()));
}

TrikScriptRunner::~TrikScriptRunner()
{
}

void TrikScriptRunner::run(QString const &script)
{
	mExecutionState = running;
	mScriptRunnerProxy->run(script);
}

void TrikScriptRunner::runFromFile(QString const &fileName)
{
	mExecutionState = running;
	mScriptRunnerProxy->run(trikKernel::FileUtils::readFromFile(fileName));
}

void TrikScriptRunner::abort()
{
	mScriptRunnerProxy->abort();
}

bool TrikScriptRunner::isInEventDrivenMode() const
{
	return mScriptRunnerProxy->isInEventDrivenMode();
}

void TrikScriptRunner::onScriptExecutionCompleted()
{
	if (mExecutionState == running) {
		mExecutionState = stopping;
		mScriptRunnerProxy->run("brick.stop()");
	} else {
		mExecutionState = idle;
		emit completed();
	}
}
