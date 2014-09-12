/* Copyright 2014 Dmitry Mordvinov, CyberTech Labs Ltd.
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

#include "threading.h"

#include "scriptEngineWorker.h"

using namespace trikScriptRunner;

int const maxThreadsCount = 100;

Threading::Threading(ScriptEngineWorker &runner)
	: mRunner(runner)
{
	QThreadPool::globalInstance()->setMaxThreadCount(maxThreadsCount);
}

void Threading::start(QScriptValue const &function)
{
	ScriptThread *thread = new ScriptThread(mScript, function.toString(), mRunner);
	thread->setAutoDelete(true);
	QThreadPool::globalInstance()->start(thread);
}

QScriptValue Threading::activeThreadCount() const
{
	return QScriptValue(QThreadPool::globalInstance()->activeThreadCount());
}

QScriptValue Threading::maxThreadCount() const
{
	return QScriptValue(QThreadPool::globalInstance()->maxThreadCount());
}

QScriptValue Threading::waitForDone(QScriptValue const &timeout)
{
	return QScriptValue(QThreadPool::globalInstance()->waitForDone(timeout.toInt32()));
}

void Threading::setCurrentScript(QString const &script)
{
	mScript = script;
}

void Threading::waitForAll()
{
	QThreadPool::globalInstance()->waitForDone();
}

Threading::ScriptThread::ScriptThread(QString const &mainScript, QString const &function, ScriptEngineWorker &runner)
	: mScript(mainScript)
	, mFunction(function)
	, mRunner(runner.clone())
{
}

void Threading::ScriptThread::run()
{
	mRunner.run(mScript, false, mFunction);
}
