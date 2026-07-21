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

#pragma once

#include <atomic>

#include <QtCore/QObject>

#include <trikControl/brickInterface.h>
#include <trikNetwork/mailboxInterface.h>

#include "trikScriptRunnerDeclSpec.h"
#include "trikScriptControlInterface.h"
#include "trikCppProgram.h"

namespace trikScriptRunner {

/// Worker object that runs a user C++ shared library in a dedicated thread.
/// Loads the .so via QLibrary, resolves the factory symbol "createTrikCppProgram",
/// instantiates the user's TrikCppProgram subclass, and calls startProgram().
class TRIKSCRIPTRUNNER_EXPORT CppEngineWorker : public QObject
{
	Q_OBJECT

public:
	CppEngineWorker(trikControl::BrickInterface *brick
			, trikNetwork::MailboxInterface *mailbox
			, TrikScriptControlInterface *scriptControl
			);

	/// Signals the running program to stop and invalidates any queued run requests.
	/// Safe to call from any thread.
	void stopScript();

	/// Resets brick state. Safe to call from any thread.
	void resetBrick();

	/// Queues a run request. Any pending but not-yet-started run is skipped.
	/// Safe to call from any thread.
	void run(const QString &fileName, int scriptId);

Q_SIGNALS:
	void completed(const QString &error, int scriptId);
	void startedScript(const QString &fileName, int scriptId);

private:
	/// Executed by the worker thread's event loop.
	/// @param runId - sequence number; if stale (a newer run was queued), this invocation is skipped.
	void doRun(const QString &fileName, int scriptId, int runId);

	trikControl::BrickInterface   *mBrick;         // no ownership
	trikNetwork::MailboxInterface *mMailbox;        // no ownership
	TrikScriptControlInterface    *mScriptControl;  // no ownership

	std::atomic<bool> mIsTerminate { false };

	/// Incremented by stopScript() and run() to invalidate stale queued invocations.
	/// Accessed from both the calling thread and the worker thread — must be atomic.
	std::atomic<int> mCurrentRunId { 0 };
};

} // namespace trikScriptRunner
