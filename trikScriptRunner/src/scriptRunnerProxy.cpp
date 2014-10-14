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

#include "scriptRunnerProxy.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>

#include "scriptEngineWorker.h"

using namespace trikScriptRunner;

ScriptRunnerProxy::ScriptRunnerProxy(trikControl::Brick &brick, QString const &startDirPath)
{
	mEngineWorker = new ScriptEngineWorker(brick, startDirPath);

	connect(&mWorkerThread, SIGNAL(finished()), mEngineWorker, SLOT(deleteLater()));
	connect(&mWorkerThread, SIGNAL(finished()), &mWorkerThread, SLOT(deleteLater()));

	mEngineWorker->moveToThread(&mWorkerThread);

	connect(mEngineWorker, SIGNAL(completed(QString)), this, SIGNAL(completed(QString)));

	mWorkerThread.start();

	QMetaObject::invokeMethod(mEngineWorker, "init");
}

ScriptRunnerProxy::~ScriptRunnerProxy()
{
	/// @todo Do not reset engine here, just shut it down.
	mEngineWorker->reset();
	QMetaObject::invokeMethod(&mWorkerThread, "quit");
	mWorkerThread.wait(1000);
}

void ScriptRunnerProxy::brickBeep()
{
	QMetaObject::invokeMethod(mEngineWorker, "brickBeep");
}

void ScriptRunnerProxy::run(QString const &script, bool inEventDrivenMode, QString const &function)
{
	QMetaObject::invokeMethod(mEngineWorker, "run"
			, Q_ARG(QString const &, script)
			, Q_ARG(bool, inEventDrivenMode)
			, Q_ARG(QString const &, function));
}

void ScriptRunnerProxy::reset()
{
	mEngineWorker->reset();
}
