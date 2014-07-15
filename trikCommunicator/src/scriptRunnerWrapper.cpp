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

#include <trikScriptRunner/trikScriptRunner.h>

using namespace trikCommunicator;

ScriptRunnerWrapper::ScriptRunnerWrapper(QString const &configFilePath, QString const &startDirPath)
	: mRunner(new trikScriptRunner::TrikScriptRunner(configFilePath, startDirPath))
	, mOwnsRunner(true)
{
	connect(mRunner, SIGNAL(completed()), this, SLOT(onScriptExecutionCompleted()));
}

ScriptRunnerWrapper::ScriptRunnerWrapper(trikScriptRunner::TrikScriptRunner &runner)
	: mRunner(&runner)
	, mOwnsRunner(false)
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
	mRunner->run(script);
}

void ScriptRunnerWrapper::stop()
{
	mRunner->abort();
}

void ScriptRunnerWrapper::onScriptExecutionCompleted()
{
	emit finishedScript();
}
