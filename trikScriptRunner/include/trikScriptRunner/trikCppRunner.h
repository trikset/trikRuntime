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

#include "trikScriptRunnerInterface.h"
#include "trikScriptControlInterface.h"

#include <QtCore/QPointer>
#include <QtCore/QThread>
#include <QtCore/QAtomicInt>

namespace trikNetwork {
class MailboxInterface;
}

namespace trikControl {
class BrickInterface;
}

namespace trikScriptRunner {

class CppEngineWorker;

/// Runs user C++ programs compiled as shared libraries (.so).
/// Mirrors the structure of TrikJavaScriptRunner / TrikPythonRunner.
class TRIKSCRIPTRUNNER_EXPORT TrikCppRunner : public TrikScriptRunnerInterface
{
	Q_OBJECT

public:
	TrikCppRunner(trikControl::BrickInterface *brick
			, trikNetwork::MailboxInterface *mailbox
			, TrikScriptControlInterface *scriptControl
			);

	~TrikCppRunner() override;

	/// Not applicable for compiled C++ programs.
	void registerUserFunction(const QString &name, QScriptEngine::FunctionSignature function) override;

	/// Not applicable for compiled C++ programs.
	void addCustomEngineInitStep(const std::function<void(QScriptEngine *)> &step) override;

	QStringList knownMethodNames() const override;

	bool wasError() override { return mHadError; }

public Q_SLOTS:
	/// @param script  - ignored (C++ programs are precompiled; pass empty string).
	/// @param fileName - path to the .so file to load and execute.
	void run(const QString &script, const QString &fileName = "") override;
	void runDirectCommand(const QString &command) override;
	void abort() override;
	void brickBeep() override;
	void setWorkingDirectory(const QString &workingDir) override;

private Q_SLOTS:
	void onStartedScript(const QString &fileName, int scriptId);
	void onCompleted(const QString &error, int scriptId);

private:
	QPointer<CppEngineWorker> mWorker; // memory managed by mWorkerThread via deleteLater
	QThread mWorkerThread;
	int mMaxScriptId { 0 };
	bool mHadError { false };
};

} // namespace trikScriptRunner
