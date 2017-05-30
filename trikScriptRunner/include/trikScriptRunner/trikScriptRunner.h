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

#pragma once

#include "trikScriptRunnerInterface.h"

namespace trikNetwork {
class MailboxInterface;
}

namespace trikControl {
class BrickInterface;
}

namespace trikScriptRunner {

class ScriptEngineWorker;
class ScriptExecutionControl;

/// Executes scripts in Qt Scripting Engine.
class TrikScriptRunner : public TrikScriptRunnerInterface
{
	Q_OBJECT
public:
	/// Constructor.
	/// @param brick - reference to trikControl::Brick instance.
	/// @param mailbox - mailbox object used to communicate with other robots.
	TrikScriptRunner(trikControl::BrickInterface &brick
					 , trikNetwork::MailboxInterface * const mailbox
					 );

	~TrikScriptRunner();

	void registerUserFunction(const QString &name, QScriptEngine::FunctionSignature function) override;
	void addCustomEngineInitStep(const std::function<void (QScriptEngine *)> &step) override;

	/// Gets all method names from executive objects (brick, script, etc.) from ScriptEngineWorker
	/// (useful when used from outside of the TrikRuntime).
	QStringList knownMethodNames() const;

public slots:
	void run(const QString &script, const QString &fileName = "") override;
	void runDirectCommand(const QString &command) override;
	void abort() override;
	void brickBeep() override;

signals:
	/// Emitted when current script completes execution (for event-driven mode it means that script requested to quit
	/// or was aborted).
	/// @param error - localized error message if any error occured during script execution or empty string
	/// if everything is fine.
	/// @param scriptId - unique identifier of a script completed
	void completed(const QString &error, int scriptId);

	/// Emitted when new script from file started.
	/// @param fileName - name of a file from where the script was loaded.
	/// @param scriptId - unique id of executed script assigned when script started.
	void startedScript(const QString &fileName, int scriptId);

	/// Emitted when new direct script started.
	/// @param scriptId - unique id of executed script assigned when script started.
	void startedDirectScript(int scriptId);

	/// Emitted when a message must be sent to a desktop.
	void sendMessage(const QString &text);

private slots:
	void onScriptStart(int scriptId);

	/// Sends message to host machine from mailbox via wifi.
	void sendMessageFromMailBox(int senderNumber, const QString &message);

private:
	QScopedPointer<ScriptExecutionControl> mScriptController;

	/// Has ownership, memory is managed by thread and deleteLater().
	ScriptEngineWorker *mScriptEngineWorker;
	QThread mWorkerThread;

	int mMaxScriptId;

	QHash<int, QString> mScriptFileNames;
};

}
