/* Copyright 2013 Yurii Litvinov
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

#include "include/trikScriptRunner/scriptRunnerProxy.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>

#include "scriptableParts.h"
#include "scriptEngineWorker.h"
#include "fileUtils.h"

using namespace trikScriptRunner;

ScriptRunnerProxy::ScriptRunnerProxy(QString const &configFilePath, QString const &startDirPath)
{
	mEngineWorker = new ScriptEngineWorker(configFilePath, startDirPath);

	connect(&mWorkerThread, SIGNAL(finished()), mEngineWorker, SLOT(deleteLater()));
	connect(&mWorkerThread, SIGNAL(finished()), &mWorkerThread, SLOT(deleteLater()));
	connect(this, SIGNAL(threadDelete()), &mWorkerThread, SLOT(quit()));

	mEngineWorker->moveToThread(&mWorkerThread);

	connect(this, SIGNAL(threadRun(QString const &)), mEngineWorker, SLOT(run(QString const &)));
	connect(mEngineWorker, SIGNAL(completed()), this, SIGNAL(completed()));

	mWorkerThread.start();
}

ScriptRunnerProxy::~ScriptRunnerProxy()
{
	mEngineWorker->abort();
	emit threadDelete();
	mWorkerThread.wait(1000);
}

void ScriptRunnerProxy::run(QString const &script)
{
	emit threadRun(script);
}

void ScriptRunnerProxy::runFromFile(QString const &fileName)
{
	emit threadRun(FileUtils::readFromFile(fileName));
}

void ScriptRunnerProxy::abort()
{
	mEngineWorker->abort();
}

bool ScriptRunnerProxy::isInEventDrivenMode() const
{
	return mEngineWorker->isInEventDrivenMode();
}
