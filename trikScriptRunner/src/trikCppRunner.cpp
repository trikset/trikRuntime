/* Copyright 2024 CyberTech Labs Ltd.
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

#include "trikCppRunner.h"
#include "src/cppEngineWorker.h"

#include <QtCore/QEventLoop>

#include <QsLog.h>

using namespace trikScriptRunner;

TrikCppRunner::TrikCppRunner(trikControl::BrickInterface *brick
		, trikNetwork::MailboxInterface *mailbox
		, TrikScriptControlInterface *scriptControl
		)
	: mWorker(new CppEngineWorker(brick, mailbox, scriptControl))
{
	mWorker->moveToThread(&mWorkerThread);

	connect(&mWorkerThread, &QThread::finished, mWorker, &QObject::deleteLater);

	connect(mWorker, &CppEngineWorker::startedScript, this, &TrikCppRunner::onStartedScript);
	connect(mWorker, &CppEngineWorker::completed,     this, &TrikCppRunner::onCompleted);

	QLOG_INFO() << "Starting TrikCppRunner worker thread" << &mWorkerThread;
	mWorkerThread.setObjectName("CppEngineWorker");
	mWorkerThread.start();
}

TrikCppRunner::~TrikCppRunner()
{
	QEventLoop wait;
	connect(&mWorkerThread, &QThread::finished, &wait, &QEventLoop::quit);

	if (mWorker) {
		mWorker->stopScript();
	}
	mWorkerThread.quit();
	wait.exec();

	constexpr auto kPoliteTimeout = 100;
	if (!mWorkerThread.wait(kPoliteTimeout)) {
		QLOG_WARN() << "C++ runner thread failed to exit gracefully in" << kPoliteTimeout
				<< "ms, re-trying with 3x timeout";
		if (!mWorkerThread.wait(3 * kPoliteTimeout)) {
			QLOG_ERROR() << "C++ runner thread failed to exit gracefully, waiting indefinitely";
			mWorkerThread.wait();
		}
	}
}

void TrikCppRunner::run(const QString & /*script*/, const QString &fileName)
{
	QLOG_INFO() << "TrikCppRunner: run" << fileName;

	if (mWorker) {
		mWorker->stopScript();
	}

	mHadError = false;
	mWorker->run(fileName, mMaxScriptId++);
}

void TrikCppRunner::abort()
{
	if (mWorker) {
		mWorker->stopScript();
		mWorker->resetBrick();
	}
}

void TrikCppRunner::onStartedScript(const QString &fileName, int scriptId)
{
	Q_EMIT startedScript(fileName, scriptId);
}

void TrikCppRunner::onCompleted(const QString &error, int scriptId)
{
	mHadError = !error.isEmpty();
	Q_EMIT completed(error, scriptId);
}

void TrikCppRunner::runDirectCommand(const QString & /*command*/)
{
	QLOG_ERROR() << "TrikCppRunner: runDirectCommand is not supported for compiled C++ programs";
}

void TrikCppRunner::brickBeep()
{
	// No script engine to beep through; ignore silently.
}

void TrikCppRunner::setWorkingDirectory(const QString & /*workingDir*/)
{
	// Not applicable: the .so path is absolute and the working dir has no effect.
}

void TrikCppRunner::registerUserFunction(const QString & /*name*/, QScriptEngine::FunctionSignature /*function*/)
{
	QLOG_ERROR() << "TrikCppRunner: registerUserFunction is not supported for compiled C++ programs";
}

void TrikCppRunner::addCustomEngineInitStep(const std::function<void(QScriptEngine *)> & /*step*/)
{
	QLOG_ERROR() << "TrikCppRunner: addCustomEngineInitStep is not supported for compiled C++ programs";
}

QStringList TrikCppRunner::knownMethodNames() const
{
	return {};
}
