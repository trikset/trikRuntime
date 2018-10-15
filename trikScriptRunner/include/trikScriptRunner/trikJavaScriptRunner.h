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
#include "trikVariablesServer.h"

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
class TrikJavaScriptRunner : public TrikScriptRunnerInterface
{
	Q_OBJECT
public:
	/// Constructor.
	/// @param brick - reference to trikControl::Brick instance.
	/// @param mailbox - mailbox object used to communicate with other robots.
	TrikJavaScriptRunner(trikControl::BrickInterface &brick
						 , trikNetwork::MailboxInterface * const mailbox
						 );

	~TrikJavaScriptRunner();

	void registerUserFunction(const QString &name, QScriptEngine::FunctionSignature function) override;
	void addCustomEngineInitStep(const std::function<void (QScriptEngine *)> &step) override;

	QStringList knownMethodNames() const override;

public slots:
	void run(const QString &script, const QString &fileName = "") override;
	void runDirectCommand(const QString &command) override;
	void abort() override;
	void brickBeep() override;

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
	QScopedPointer<TrikVariablesServer> mVariablesServer;
};

}
