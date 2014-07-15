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

#include "src/scriptRunnerWrapper.h"

#include <trikScriptRunner/scriptRunnerProxy.h>

using namespace trikCommunicator;

ScriptRunnerWrapper::ScriptRunnerWrapper(QString const &configFilePath, QString const &startDirPath)
	: mRunner(new trikScriptRunner::ScriptRunnerProxy(configFilePath, startDirPath))
	, mOwnsRunner(true)
	, mExecutionState(idle)
{
	connect(mRunner, SIGNAL(completed()), this, SLOT(onScriptExecutionCompleted()));
}

ScriptRunnerWrapper::ScriptRunnerWrapper(trikScriptRunner::ScriptRunnerProxy &runner)
	: mRunner(&runner)
	, mOwnsRunner(false)
	, mExecutionState(idle)
{
	connect(mRunner, SIGNAL(completed()), this, SLOT(onScriptExecutionCompleted()));
}

ScriptRunnerWrapper::~ScriptRunnerWrapper()
{
	if (mOwnsRunner) {
		delete mRunner;
	}
}

void ScriptRunnerWrapper::run(QString const &script)
{
	mExecutionState = running;
	mRunner->run(script);
}

void ScriptRunnerWrapper::stop()
{
	mExecutionState = stopping;
	mRunner->abort();
	mRunner->run("brick.stop()");
}

void ScriptRunnerWrapper::onScriptExecutionCompleted()
{
	if (mExecutionState == running) {
		mExecutionState = stopping;
		mRunner->run("brick.stop()");
	} else {
		mExecutionState = idle;
		emit finishedScript();
	}
}
