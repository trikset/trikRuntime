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

TrikScriptRunner::TrikScriptRunner(trikControl::Brick &brick, QString const &startDirPath)
	: mScriptRunnerProxy(new ScriptRunnerProxy(brick, startDirPath))
{
	connect(mScriptRunnerProxy.data(), SIGNAL(completed(QString))
			, this, SIGNAL(completed(QString)));
}

TrikScriptRunner::~TrikScriptRunner()
{
}

void TrikScriptRunner::brickBeep()
{
	mScriptRunnerProxy->brickBeep();
}

void TrikScriptRunner::run(QString const &script)
{
	mScriptRunnerProxy->run(script, false);
}

void TrikScriptRunner::runDirectCommand(QString const &command)
{
	mScriptRunnerProxy->run(command, true, QString());
}

void TrikScriptRunner::abort()
{
	mScriptRunnerProxy->reset();
}
