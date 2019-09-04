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

int TrikPyRunnerTest::run(const QString &script, const QString &file)
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
	QTimer::singleShot(10000, &wait, std::bind(&QEventLoop::exit, &wait, -1));
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

	return run(fileContents, fileName);
}

trikScriptRunner::TrikScriptRunner &TrikPyRunnerTest::scriptRunner()
{
	return *mScriptRunner;
}

TEST_F(TrikPyRunnerTest, sanityCheckPy)
{
	run("1 + 1", "_.py");
}

TEST_F(TrikPyRunnerTest, DISABLED_fileTestPy)
{
	runFromFile("file-test.py");
}

TEST_F(TrikPyRunnerTest, pythonAccessQtCore)
{
	run("from PythonQt import QtCore\nQtCore.QTimer.singleShot(500)", "_.py");
}

TEST_F(TrikPyRunnerTest, pythonScriptWait)
{
	scriptRunner().run("script.wait(500)", "_.py");
	tests::utils::Wait::wait(600);
}
