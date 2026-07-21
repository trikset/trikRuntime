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

#include "cppEngineWorker.h"

#include <memory>

#include <QtCore/QLibrary>

#include <QsLog.h>

using namespace trikScriptRunner;

CppEngineWorker::CppEngineWorker(trikControl::BrickInterface *brick
		, trikNetwork::MailboxInterface *mailbox
		, TrikScriptControlInterface *scriptControl
		)
	: mBrick(brick)
	, mMailbox(mailbox)
	, mScriptControl(scriptControl)
{
}

void CppEngineWorker::stopScript()
{
	// Increment first so that any doRun already queued sees a stale runId and skips itself.
	mCurrentRunId.fetch_add(1, std::memory_order_relaxed);
	mIsTerminate = true;
}

void CppEngineWorker::resetBrick()
{
	if (mBrick) {
		mBrick->reset();
	}
}

void CppEngineWorker::run(const QString &fileName, int scriptId)
{
	// Increment and capture the new runId. Any doRun queued earlier now has a stale id.
	const int runId = mCurrentRunId.fetch_add(1, std::memory_order_relaxed) + 1;

	QMetaObject::invokeMethod(this, [this, fileName, scriptId, runId]() {
		doRun(fileName, scriptId, runId);
	}, Qt::QueuedConnection);
}

void CppEngineWorker::doRun(const QString &fileName, int scriptId, int runId)
{
	// Another run() or stopScript() was called after we were queued — skip.
	if (runId != mCurrentRunId.load(std::memory_order_relaxed)) {
		QLOG_INFO() << "CppEngineWorker: scriptId" << scriptId << "superseded, skipping";
		Q_EMIT completed("", scriptId);
		return;
	}

	QLOG_INFO() << "CppEngineWorker: loading" << fileName;

	QLibrary lib(fileName);
	if (!lib.load()) {
		const QString error = tr("Cannot load shared library '%1': %2").arg(fileName, lib.errorString());
		QLOG_ERROR() << error;
		Q_EMIT completed(error, scriptId);
		return;
	}

	auto factory = reinterpret_cast<TrikCppProgramFactory>(
			lib.resolve(TRIK_CPP_PROGRAM_FACTORY_SYMBOL));
	if (!factory) {
		const QString error = tr("Symbol '%1' not found in '%2'")
				.arg(TRIK_CPP_PROGRAM_FACTORY_SYMBOL, fileName);
		QLOG_ERROR() << error;
		lib.unload();
		Q_EMIT completed(error, scriptId);
		return;
	}

	std::unique_ptr<TrikCppProgram> program(factory(mBrick, mMailbox, mScriptControl));
	if (!program) {
		const QString error = tr("Factory '%1' returned nullptr in '%2'")
				.arg(TRIK_CPP_PROGRAM_FACTORY_SYMBOL, fileName);
		QLOG_ERROR() << error;
		lib.unload();
		Q_EMIT completed(error, scriptId);
		return;
	}

	mIsTerminate = false;

	Q_EMIT startedScript(fileName, scriptId);

	QString errorMessage;
	try {
		program->startProgram(mIsTerminate);
	} catch (const std::exception &e) {
		errorMessage = QString::fromLatin1(e.what());
		QLOG_ERROR() << "CppEngineWorker: exception in user program:" << errorMessage;
	} catch (...) {
		errorMessage = tr("Unknown exception in C++ program");
		QLOG_ERROR() << errorMessage;
	}

	// Destroy the program object before unloading the library —
	// the destructor lives in the user's .so and must be called while it is still mapped.
	program.reset();
	lib.unload();

	QLOG_INFO() << "CppEngineWorker: program finished, scriptId=" << scriptId;
	Q_EMIT completed(errorMessage, scriptId);
}
