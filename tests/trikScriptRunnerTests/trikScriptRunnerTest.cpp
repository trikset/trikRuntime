/* Copyright 2014 Cybertech Labs Ltd.
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

#include "trikScriptRunnerTest.h"

#include <QtWidgets/QApplication>

using namespace tests;

void TrikScriptRunnerTest::SetUp()
{
}

void TrikScriptRunnerTest::TearDown()
{
}

TEST_F(TrikScriptRunnerTest, sanityCheck)
{
	int argc = 0;
	char *argv = "";
	QApplication app(argc, &argv);
	trikControl::Brick brick(*QThread::currentThread(), "./", "./");
	trikScriptRunner::TrikScriptRunner scriptRunner(brick, "./");
	scriptRunner.run("1 + 1");
}
