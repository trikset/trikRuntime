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
#include <QFileInfo>
#include <QMutex>
#include <QScopedPointer>
#include <array>
#include <trikControl/brickInterface.h>
#include <trikNetwork/mailboxInterface.h>

namespace trikScriptRunner {

/// General wrapper for other executors (such as Python, JavaScript)
class TRIKSCRIPTRUNNER_EXPORT TrikScriptRunner : public TrikScriptRunnerInterface
{
	Q_OBJECT
public:
	/// Constructor.
	/// @param brick - reference to trikControl::Brick instance.
	/// @param mailbox - mailbox object used to communicate with other robots.
	/// @param scriptControl - implementation of script object
	TrikScriptRunner(trikControl::BrickInterface &brick
					 , trikNetwork::MailboxInterface * mailbox
					 , TrikScriptControlInterface * scriptControl
					 );

	/// Constructor.
	/// @param brick - reference to trikControl::Brick instance.
	/// @param mailbox - mailbox object used to communicate with other robots.
	TrikScriptRunner(trikControl::BrickInterface &brick
					 , trikNetwork::MailboxInterface * mailbox
					 );

	~TrikScriptRunner() override;

	/// Choose default runner type (Python or JavaScript)
	void setDefaultRunner(ScriptType t);

	/// Choose default runner type (Python or JavaScript) from extension
	void setDefaultRunner(const QString &languageExtension);

	void registerUserFunction(const QString &name, QScriptEngine::FunctionSignature function) override;
	void addCustomEngineInitStep(const std::function<void (QScriptEngine *)> &step) override;
	bool wasError() override;

	/// Create completion list for interpreted language
	QStringList knownMethodNamesFor(ScriptType t);

	/// See corresponding TrikScriptRunnerInterface method
	QStringList knownMethodNames() const override;
	/// Execute script with a corresponding engine of specified type
	void run(const QString &script, ScriptType stype, const QString &fileName = "");
signals:
	/// Broadcasts message to all opened mailboxes.
	void sendMailboxMessage(QString msg);
public slots:
	/// See corresponding TrikScriptRunnerInterface method
	void run(const QString &script, const QString &fileName = "") override;
	/// See corresponding TrikScriptRunnerInterface method
	void runDirectCommand(const QString &command) override;
	/// See corresponding TrikScriptRunnerInterface method
	void abort() override;
	/// Aborts all scripts from all engines.
	void abortAll();
	/// See corresponding TrikScriptRunnerInterface method
	void brickBeep() override;

	void setWorkingDirectory(const QString &workingDir) override;

private:
	TrikScriptRunnerInterface * fetchRunner(ScriptType stype);

	trikControl::BrickInterface &mBrick;
	trikNetwork::MailboxInterface * mMailbox;
	QSharedPointer <TrikScriptControlInterface> mScriptControl;
	std::vector<QSharedPointer<TrikScriptRunnerInterface>> mScriptRunnerArray;
	ScriptType mLastRunner;
};

}
