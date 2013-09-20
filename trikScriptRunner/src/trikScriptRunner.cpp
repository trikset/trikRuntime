/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

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
