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

#include <QtCore/QEventLoop>
#include <QtCore/QFile>

#include <trikControl/brickFactory.h>
#include <trikKernel/fileUtils.h>
#include <testUtils/wait.h>

using namespace tests;

QScriptValue scriptAssert(QScriptContext *context, QScriptEngine *engine)
{
	Q_UNUSED(engine);

	if (context->argumentCount() != 1) {
		ADD_FAILURE() << "'assert' shall have exactly one argument";
		return {};
	}

	if (!context->argument(0).toBool()) {
		ADD_FAILURE() << "Assertion failure at\n"
				<< QStringList(context->backtrace().mid(1)).join("\n").toStdString();
	}

	return {};
}

void TrikScriptRunnerTest::SetUp()
{
	mBrick.reset(trikControl::BrickFactory::create("./", "./"));
	mScriptRunner.reset(new trikScriptRunner::TrikScriptRunner(*mBrick, nullptr));
	mScriptRunner->registerUserFunction("assert", scriptAssert);
}

void TrikScriptRunnerTest::TearDown()
{
}

void TrikScriptRunnerTest::run(const QString &script)
{
	QEventLoop waitingLoop;
	QObject::connect(mScriptRunner.data(), SIGNAL(completed(QString, int)), &waitingLoop, SLOT(quit()));
	mScriptRunner->run(script);

	waitingLoop.exec();
}

void TrikScriptRunnerTest::runDirectCommandAndWaitForQuit(const QString &script)
{
	QEventLoop waitingLoop;
	QObject::connect(mScriptRunner.data(), SIGNAL(completed(QString, int)), &waitingLoop, SLOT(quit()));
	mScriptRunner->runDirectCommand(script);

	waitingLoop.exec();
}

void TrikScriptRunnerTest::runFromFile(const QString &fileName)
{
	const auto fileContents = trikKernel::FileUtils::readFromFile("data/" + fileName);
	run(fileContents);
}

trikScriptRunner::TrikScriptRunner &TrikScriptRunnerTest::scriptRunner()
{
	return *mScriptRunner;
}

TEST_F(TrikScriptRunnerTest, sanityCheck)
{
	run("1 + 1");
}

TEST_F(TrikScriptRunnerTest, fileTest)
{
	runFromFile("file-test.js");
}

TEST_F(TrikScriptRunnerTest, asyncSystemTest)
{
	QFile testFile("test");
	testFile.remove();
	ASSERT_FALSE(testFile.exists());
	runFromFile("async-system-test.js");
	ASSERT_FALSE(testFile.exists());
	tests::utils::Wait::wait(2500);
	ASSERT_TRUE(testFile.exists());
}

TEST_F(TrikScriptRunnerTest, syncSystemTest)
{
	QFile testFile("test");
	testFile.remove();
	ASSERT_FALSE(testFile.exists());
	runFromFile("sync-system-test.js");
	ASSERT_TRUE(testFile.exists());
}

TEST_F(TrikScriptRunnerTest, directCommandTest)
{
	QFile testFile("test");
	testFile.remove();
	ASSERT_FALSE(testFile.exists());
	scriptRunner().runDirectCommand("script.system('echo 123 > test', true);");
	tests::utils::Wait::wait(100);
	ASSERT_TRUE(testFile.exists());
	scriptRunner().runDirectCommand("script.system('rm test', true);");
	tests::utils::Wait::wait(100);
	ASSERT_FALSE(testFile.exists());
	scriptRunner().runDirectCommand("script.quit();");
}

TEST_F(TrikScriptRunnerTest, directCommandThatQuitsImmediatelyTest)
{
	QFile testFile("test");
	testFile.remove();
	ASSERT_FALSE(testFile.exists());
	runDirectCommandAndWaitForQuit("script.system('echo 123 > test', true); script.quit();");
	ASSERT_TRUE(testFile.exists());
	runDirectCommandAndWaitForQuit("script.system('rm test', true); script.quit();");
	ASSERT_FALSE(testFile.exists());
}

TEST_F(TrikScriptRunnerTest, twoProgramsTest)
{
	scriptRunner().run("script.wait(500);");
	tests::utils::Wait::wait(100);
	scriptRunner().run("script.wait(500);");
	tests::utils::Wait::wait(600);
}
