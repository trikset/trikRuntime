/* Copyright 2014 - 2015 CyberTech Labs Ltd.
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

#include <trikControl/brickInterface.h>
#include <trikScriptRunner/trikScriptRunner.h>
#include <trikKernel/deinitializationHelper.h>

#include <gtest/gtest.h>

namespace tests {

/// Test suite for script runner.
class TrikScriptRunnerTest : public testing::Test
{
protected:
	void SetUp() override;
	void TearDown() override;

	void run(const QString &script, const QString &file);
	void runDirectCommandAndWaitForQuit(const QString &script);
	void runFromFile(const QString &fileName);

	trikScriptRunner::TrikScriptRunner &scriptRunner();

private:
	/// Does nothing, but ensures event processing at the time of destruction of test suite, to avoid
	/// deleteLater()-related memleaks.
	trikKernel::DeinitializationHelper mHelper;

	QScopedPointer<trikControl::BrickInterface> mBrick;
	QScopedPointer<trikScriptRunner::TrikScriptRunner> mScriptRunner;
};

}
