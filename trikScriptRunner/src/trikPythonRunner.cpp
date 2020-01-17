/* Copyright 2018 Iakov Kirilenko, CyberTech Labs Ltd.
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

#include <QEventLoop>
#include <QsLog.h>

#include "trikPythonRunner.h"

#include "src/pythonEngineWorker.h"
#include <stdexcept>

using namespace trikScriptRunner;

TrikPythonRunner::TrikPythonRunner(trikControl::BrickInterface &brick
								   , trikNetwork::MailboxInterface * const mailbox
								   )
	:	mScriptEngineWorker(new PythonEngineWorker(brick, mailbox))
{
	mScriptEngineWorker->moveToThread(&mWorkerThread);
	connect(&mWorkerThread, &QThread::finished, mScriptEngineWorker, &PythonEngineWorker::deleteLater);
	connect(&mWorkerThread, &QThread::started, mScriptEngineWorker, &PythonEngineWorker::init);
	connect(mScriptEngineWorker, &PythonEngineWorker::textInStdOut, this, &TrikPythonRunner::textInStdOut);
	connect(mScriptEngineWorker, &PythonEngineWorker::completed, this, &TrikPythonRunner::completed);
	connect(mScriptEngineWorker, &PythonEngineWorker::startedScript, this, &TrikPythonRunner::startedScript);
	connect(mScriptEngineWorker, &PythonEngineWorker::startedDirectScript
			, this, &TrikPythonRunner::startedDirectScript);

	QEventLoop l;
	connect(mScriptEngineWorker, &PythonEngineWorker::inited, &l, &QEventLoop::quit);
	QLOG_INFO() << "Starting TrikPythonRunner worker thread" << &mWorkerThread;
	mWorkerThread.start();
	l.exec();
}

TrikPythonRunner::~TrikPythonRunner()
{
	mScriptEngineWorker->stopScript();
	mWorkerThread.quit();
	mWorkerThread.wait(1000);
}

void TrikPythonRunner::run(const QString &script, const QString &fileName)
{
	QFileInfo scriptFile = QFileInfo(fileName);
	mScriptEngineWorker->stopScript();
	mScriptEngineWorker->run(script, scriptFile);
}

void TrikPythonRunner::registerUserFunction(const QString &name, QScriptEngine::FunctionSignature function)
{
	Q_UNUSED(name)
	Q_UNUSED(function)
	throw std::logic_error("Not implemented");
}

void TrikPythonRunner::addCustomEngineInitStep(const std::function<void (QScriptEngine *)> &step)
{
	Q_UNUSED(step)
	throw std::logic_error("Not implemented");
}

void TrikPythonRunner::brickBeep()
{
	QMetaObject::invokeMethod(mScriptEngineWorker, &PythonEngineWorker::brickBeep);
}

void TrikPythonRunner::runDirectCommand(const QString &command)
{
	QLOG_INFO() << "TrikPythonRunner: new direct command" << command;
	mScriptEngineWorker->runDirect(command);
}

void TrikPythonRunner::abort()
{
	mScriptEngineWorker->stopScript();
	mScriptEngineWorker->resetBrick();
}

QStringList TrikPythonRunner::knownMethodNames() const
{
	return mScriptEngineWorker->knownNames();
}

bool TrikPythonRunner::wasError()
{
	return PythonQt::self()->hadError();
}
