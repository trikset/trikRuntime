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

#include "trikScriptRunnerTest.h"

#include <QtCore/QScopedPointer>

#include <trikControl/brickInterface.h>
#include <trikControl/brickFactory.h>
#include <trikNetwork/gamepadFactory.h>
#include <trikNetwork/gamepadInterface.h>
#include <trikNetwork/mailboxFactory.h>
#include <trikNetwork/mailboxInterface.h>

using namespace tests;

void TrikScriptRunnerTest::SetUp()
{
}

void TrikScriptRunnerTest::TearDown()
{
}

TEST_F(TrikScriptRunnerTest, sanityCheck)
{
	QScopedPointer<trikControl::BrickInterface> brick(
			trikControl::BrickFactory::create(*QThread::currentThread(), "../../trikControl/", "./"));

	trikScriptRunner::TrikScriptRunner scriptRunner(*brick, nullptr, nullptr, "./");
	scriptRunner.run("1 + 1");
}
