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

#include "include/trikScriptRunner/trikScriptRunner.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>

#include "scriptableParts.h"
#include "scriptEngineWorker.h"

using namespace trikScriptRunner;

TrikScriptRunner::TrikScriptRunner()
{
	mEngineWorker = new ScriptEngineWorker();

	mEngineWorker->moveToThread(&mWorkerThread);

	connect(this, SIGNAL(threadRun(QString const &)), mEngineWorker, SLOT(run(QString const &)));
	connect(this, SIGNAL(threadDelete()), mEngineWorker, SLOT(deleteWorker()));

	mWorkerThread.start();
}

TrikScriptRunner::~TrikScriptRunner()
{
	mEngineWorker->abort();
	emit threadDelete();
	mWorkerThread.wait(1000);
}

void TrikScriptRunner::run(QString const &script)
{
	emit threadRun(script);
}

void TrikScriptRunner::runFromFile(QString const &fileName, bool synchronous)
{
	QFile file(fileName);
	file.open(QIODevice::ReadOnly | QIODevice::Text);
	if (!file.isOpen()) {
		throw "Can not open file for reading";
	}

	QTextStream input;
	input.setDevice(&file);
	input.setCodec("UTF-8");
	QString const script = input.readAll();
	file.close();

	if (!synchronous) {
		emit threadRun(script);
	} else {
		ScriptEngineWorker temporaryWorker;
		temporaryWorker.run(script);
	}
}

void TrikScriptRunner::abort()
{
	mEngineWorker->abort();
}
