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

#include "trikScriptControlInterface.h"
#include "trikScriptRunnerInterface.h"
#include "trikVariablesServer.h"

#include <QPointer>

namespace trikNetwork {
class MailboxInterface;
}

namespace trikControl {
class BrickInterface;
}

namespace trikScriptRunner {

class ScriptEngineWorker;
class TrikScriptControlInterface;

/// Executes scripts in Qt Scripting Engine.
class TRIKSCRIPTRUNNER_EXPORT TrikJavaScriptRunner : public TrikScriptRunnerInterface
{
	Q_OBJECT
public:
	/// Constructor.
	/// @param brick - reference to trikControl::Brick instance.
	/// @param mailbox - mailbox object used to communicate with other robots.
	TrikJavaScriptRunner(trikControl::BrickInterface *brick, trikNetwork::MailboxInterface * mailbox
						 , QSharedPointer<TrikScriptControlInterface> scriptControl);

	~TrikJavaScriptRunner() override;

	void registerUserFunction(const QString &name, QScriptEngine::FunctionSignature function) override;
	void addCustomEngineInitStep(const std::function<void (QScriptEngine *)> &step) override;

	QStringList knownMethodNames() const override;

	bool wasError() override { return false; }

public slots:
	void run(const QString &script, const QString &fileName = "") override;
	void runDirectCommand(const QString &command) override;
	void abort() override;
	void brickBeep() override;
	void setWorkingDirectory(const QString &workingDir) override;

private slots:
	void onScriptStart(int scriptId);

private:
	QSharedPointer<TrikScriptControlInterface> mScriptController;

	/// Has ownership, memory is managed by thread and deleteLater().
	QPointer<ScriptEngineWorker> mScriptEngineWorker;
	QThread mWorkerThread;

	int mMaxScriptId;

	QHash<int, QString> mScriptFileNames;
	QScopedPointer<TrikVariablesServer> mVariablesServer;
};

}
