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

#include "scriptThread.h"

#include <QtCore/QEventLoop>

#include "threading.h"
#include <QsLog.h>

using namespace trikScriptRunner;

ScriptThread::ScriptThread(Threading &threading, const QString &id, QScriptEngine *engine, const QString &script)
	: mId(id)
	, mEngine(engine)
	, mScript(script)
	, mThreading(threading)
{
}

ScriptThread::~ScriptThread()
{
}

void ScriptThread::run()
{
	QLOG_INFO() << "Started thread" << this;

	mEngine->evaluate(mScript);
	if (mEngine->hasUncaughtException()) {
		const int line = mEngine->uncaughtExceptionLineNumber();
		const QString message = mEngine->uncaughtException().toString();
		mError = tr("Line %1: %2").arg(QString::number(line), message);
		QLOG_ERROR() << "Uncaught exception at line" << line << ":" << message;
	} else if (mThreading.inEventDrivenMode()) {
		QEventLoop loop;
		connect(this, SIGNAL(stopRunning()), &loop, SLOT(quit()), Qt::DirectConnection);
		loop.exec();
	}

	mEngine->deleteLater();
	mThreading.threadFinished(mId);
	QLOG_INFO() << "Ended evaluation, thread" << this;
}

void ScriptThread::abort()
{
	mEngine->abortEvaluation();
	emit stopRunning();
}

QString ScriptThread::id() const
{
	return mId;
}

QString ScriptThread::error() const
{
	return mError;
}

bool ScriptThread::isEvaluating() const
{
	return mEngine->isEvaluating();
}
