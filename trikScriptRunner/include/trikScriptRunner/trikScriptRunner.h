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
#include <QFileInfo>
#include <QScopedPointer>
#include <array>

namespace trikNetwork {
class MailboxInterface;
}

namespace trikControl {
class BrickInterface;
}

namespace trikScriptRunner {

/// General wrapper for other executors (such as Python, JavaScript)
class TRIKSCRIPTRUNNER_EXPORT TrikScriptRunner : public TrikScriptRunnerInterface
{
	Q_OBJECT
public:
	/// Constructor.
	/// @param brick - reference to trikControl::Brick instance.
	/// @param mailbox - mailbox object used to communicate with other robots.
	TrikScriptRunner(trikControl::BrickInterface &brick
					 , trikNetwork::MailboxInterface * mailbox
					 );

	~TrikScriptRunner() override;

	void registerUserFunction(const QString &name, QScriptEngine::FunctionSignature function) override;
	void addCustomEngineInitStep(const std::function<void (QScriptEngine *)> &step) override;

	/// Create completion list for interpreted language
	QStringList knownMethodNamesFor(ScriptType t);

	/// See corresponding TrikScriptRunnerInterface method
	QStringList knownMethodNames() const override;
	/// Execute script with a corresponding engine of specified type
	void run(const QString &script, ScriptType stype, const QString &fileName = "");

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

private:
	TrikScriptRunnerInterface * fetchRunner(ScriptType stype);

	trikControl::BrickInterface &brick;
	trikNetwork::MailboxInterface * mailbox;
	std::array<QScopedPointer<TrikScriptRunnerInterface>, to_underlying(ScriptType::Size)> mScriptRunnerArray;
	ScriptType mLastRunner;
};

}
