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

#include "trikPyRunnerTest.h"

#include <QtCore/QEventLoop>
#include <QtCore/QFile>

#include <trikControl/brickFactory.h>
#include <trikKernel/fileUtils.h>
#include <testUtils/wait.h>
#include <QTimer>

using namespace tests;
constexpr auto EXIT_TIMEOUT = -93;

void TrikPyRunnerTest::SetUp()
{
	mBrick.reset(trikControl::BrickFactory::create("./test-system-config.xml"
				   , "./test-model-config.xml", "./media/"));
	mScriptRunner.reset(new trikScriptRunner::TrikScriptRunner(*mBrick, nullptr));
// TODO:	mScriptRunner->registerUserFunction("assert", scriptAssert);
}

void TrikPyRunnerTest::TearDown()
{
}

int TrikPyRunnerTest::run(const QString &script)
{
	QEventLoop wait;
	auto volatile alreadyCompleted = false;
	QObject::connect(mScriptRunner.data(), &trikScriptRunner::TrikScriptRunner::completed, &wait, &QEventLoop::quit);
	QObject::connect(mScriptRunner.data(), &trikScriptRunner::TrikScriptRunner::completed
					 , &wait, [&alreadyCompleted]()
	{
		alreadyCompleted = true;
	} ) ;
	QTimer::singleShot(10000, &wait, std::bind(&QEventLoop::exit, &wait, EXIT_TIMEOUT));

	mScriptRunner->run(script, "_.py");
	auto exitCode = 0;
	if (!alreadyCompleted) {
		exitCode = wait.exec();
	}
	return exitCode;
}

int TrikPyRunnerTest::runDirectCommandAndWaitForQuit(const QString &script)
{
	QEventLoop wait;
	auto volatile alreadyCompleted = false;
	QObject::connect(mScriptRunner.data(), &trikScriptRunner::TrikScriptRunner::completed, &wait, &QEventLoop::quit);
	QObject::connect(mScriptRunner.data(), &trikScriptRunner::TrikScriptRunner::completed
					 , &wait, [&alreadyCompleted]()
	{
		alreadyCompleted = true;
	} ) ;
	QTimer::singleShot(10000, &wait, std::bind(&QEventLoop::exit, &wait, EXIT_TIMEOUT));
	mScriptRunner->runDirectCommand(script);

	auto exitCode = 0;
	if (!alreadyCompleted) {
		exitCode = wait.exec();
	}
	return exitCode;
}

int TrikPyRunnerTest::runFromFile(const QString &fileName)
{
	auto fileContents = trikKernel::FileUtils::readFromFile("data/" + fileName);

#ifdef Q_OS_WIN
	fileContents = fileContents.replace("&&", ";");
#endif

	return run(fileContents);
}

trikScriptRunner::TrikScriptRunner &TrikPyRunnerTest::scriptRunner()
{
	return *mScriptRunner;
}

TEST_F(TrikPyRunnerTest, abortBeforeRun)
{
	scriptRunner().abortAll();
}


TEST_F(TrikPyRunnerTest, sanityCheckPy)
{
	auto err = run("1 + 1");
	ASSERT_EQ(err, 0);
}

TEST_F(TrikPyRunnerTest, abortWhileTrue)
{
	QTimer t;
	t.setInterval(200);
	t.setSingleShot(true);
	QObject::connect(&t, &QTimer::timeout, &scriptRunner(), &trikScriptRunner::TrikScriptRunnerInterface::abort);
	t.start();
	auto err = run("while True: pass");
	ASSERT_NE(err, EXIT_TIMEOUT);
	t.stop();
}


TEST_F(TrikPyRunnerTest, DISABLED_fileTestPy)
{
	auto err = runFromFile("file-test.py");
	ASSERT_EQ(err, 0);
}

TEST_F(TrikPyRunnerTest, pythonAccessQtCore)
{
	auto err = run("from PythonQt import QtCore\nQtCore.QTimer.singleShot(500)");
	ASSERT_EQ(err, 0);
}

TEST_F(TrikPyRunnerTest, pythonScriptWait)
{
	scriptRunner().run("script.wait(500)");
	tests::utils::Wait::wait(600);
}
