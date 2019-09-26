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

#include "trikJsRunnerTest.h"

#include <QtCore/QEventLoop>
#include <QtCore/QFile>

#include <trikControl/brickFactory.h>
#include <trikKernel/fileUtils.h>
#include <testUtils/wait.h>
#include <QTimer>

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

void TrikJsRunnerTest::SetUp()
{
	mBrick.reset(trikControl::BrickFactory::create("./test-system-config.xml"
				   , "./test-model-config.xml", "./media/"));
	mScriptRunner.reset(new trikScriptRunner::TrikScriptRunner(*mBrick, nullptr));
	mScriptRunner->registerUserFunction("assert", scriptAssert);
}

void TrikJsRunnerTest::TearDown()
{
}

int TrikJsRunnerTest::run(const QString &script, const QString &file)
{
	QEventLoop wait;
	auto volatile alreadyCompleted = false;
	QObject::connect(mScriptRunner.data(), &trikScriptRunner::TrikScriptRunner::completed, &wait, &QEventLoop::quit);
	QObject::connect(mScriptRunner.data(), &trikScriptRunner::TrikScriptRunner::completed
					 , &wait, [&alreadyCompleted]()
	{
		alreadyCompleted = true;
	} ) ;
	QTimer::singleShot(10000, &wait, std::bind(&QEventLoop::exit, &wait, -1));

	mScriptRunner->run(script, file);
	auto exitCode = 0;
	if (!alreadyCompleted) {
		exitCode = wait.exec();
	}
	return exitCode;
}

int TrikJsRunnerTest::runDirectCommandAndWaitForQuit(const QString &script)
{
	QEventLoop wait;
	auto volatile alreadyCompleted = false;
	QObject::connect(mScriptRunner.data(), &trikScriptRunner::TrikScriptRunner::completed, &wait, &QEventLoop::quit);
	QObject::connect(mScriptRunner.data(), &trikScriptRunner::TrikScriptRunner::completed
					 , &wait, [&alreadyCompleted]()
	{
		alreadyCompleted = true;
	} ) ;
	QTimer::singleShot(10000, &wait, std::bind(&QEventLoop::exit, &wait, -1));
	mScriptRunner->runDirectCommand(script);

	auto exitCode = 0;
	if (!alreadyCompleted) {
		exitCode = wait.exec();
	}
	return exitCode;
}

int TrikJsRunnerTest::runFromFile(const QString &fileName)
{
	auto fileContents = trikKernel::FileUtils::readFromFile("data/" + fileName);

#ifdef Q_OS_WIN
	fileContents = fileContents.replace("&&", ";");
#endif

	return run(fileContents, fileName);
}

trikScriptRunner::TrikScriptRunner &TrikJsRunnerTest::scriptRunner()
{
	return *mScriptRunner;
}

TEST_F(TrikJsRunnerTest, sanityCheckJs)
{
	auto errCode = run("1", "_.js");
	ASSERT_EQ(errCode, 0);
}

TEST_F(TrikJsRunnerTest, brickInterfaceAccess)
{
	auto errCode = run("1 + 1", "_.js");
	ASSERT_EQ(errCode, 0);
	const auto &knownMethodNames = scriptRunner().knownMethodNames();
	ASSERT_TRUE(knownMethodNames.contains("brick"));
	ASSERT_TRUE(knownMethodNames.contains("setPower"));
	errCode = run("brick.sensor(A1).read()", "_.js");
	ASSERT_EQ(errCode, 0);
}

TEST_F(TrikJsRunnerTest, fileTestJs)
{
	auto errCode = runFromFile("file-test.js");
	ASSERT_EQ(errCode, 0);
}

#ifndef Q_OS_WIN
TEST_F(TrikJsRunnerTest, asyncSystemTest)
{
	QFile testFile("test");
	testFile.remove();
	ASSERT_FALSE(testFile.exists());
	runFromFile("async-system-test.js");
	ASSERT_FALSE(testFile.exists());
	tests::utils::Wait::wait(2500);
	ASSERT_TRUE(testFile.exists());
}
#endif

TEST_F(TrikJsRunnerTest, syncSystemTest)
{
	QFile testFile("test");
	testFile.remove();
	ASSERT_FALSE(testFile.exists());
	runFromFile("sync-system-test.js");
	ASSERT_TRUE(testFile.exists());
}

TEST_F(TrikJsRunnerTest, directCommandTest)
{
	QFile testFile("test");
	testFile.remove();
	ASSERT_FALSE(testFile.exists());
	scriptRunner().runDirectCommand("script.system('echo 123 > test', true);");
	tests::utils::Wait::wait(300);
	ASSERT_TRUE(testFile.exists());

#ifdef Q_OS_WIN
	scriptRunner().runDirectCommand("script.system('DEL test', true);");
#else
	scriptRunner().runDirectCommand("script.system('rm test', true);");
#endif

	tests::utils::Wait::wait(300);
	ASSERT_FALSE(testFile.exists());
	scriptRunner().runDirectCommand("script.quit();");
	tests::utils::Wait::wait(300);
}

TEST_F(TrikJsRunnerTest, directCommandThatQuitsImmediatelyTest)
{
	QFile testFile("test");
	testFile.remove();
	ASSERT_FALSE(testFile.exists());
	auto exitCode = runDirectCommandAndWaitForQuit("script.system('echo 123 > test', true); script.quit();");
	ASSERT_EQ(exitCode, 0);
	ASSERT_TRUE(testFile.exists());

#ifdef Q_OS_WIN
	runDirectCommandAndWaitForQuit("script.system('DEL test', true); script.quit();");
#else
	runDirectCommandAndWaitForQuit("script.system('rm test', true); script.quit();");
#endif

	ASSERT_FALSE(testFile.exists());
}

TEST_F(TrikJsRunnerTest, twoProgramsTest)
{
	scriptRunner().run("script.wait(500);");
	tests::utils::Wait::wait(100);
	scriptRunner().run("script.wait(500);");
	tests::utils::Wait::wait(600);
}
