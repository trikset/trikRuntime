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
#include <QCoreApplication>
#include <QTimer>

using namespace tests;
constexpr auto EXIT_TIMEOUT = -93;
constexpr auto EXIT_SCRIPT_ERROR = 113;
constexpr auto EXIT_SCRIPT_SUCCESS = EXIT_SUCCESS;

void TrikPyRunnerTest::SetUp()
{
	mBrick.reset(trikControl::BrickFactory::create("./test-system-config.xml"
				   , "./test-model-config.xml", "./media/"));
	mScriptRunner.reset(new trikScriptRunner::TrikScriptRunner(*mBrick, nullptr));
	mScriptRunner->setDefaultRunner(trikScriptRunner::ScriptType::PYTHON);
	QObject::connect(&*mScriptRunner, &trikScriptRunner::TrikScriptRunnerInterface::textInStdOut,
					 &*mScriptRunner, [this](const QString &m) {
		std::cout << "Incoming:" << m.toStdString() << std::endl;
		mStdOut += m;
	});
// TODO:	mScriptRunner->registerUserFunction("assert", scriptAssert);
}

void TrikPyRunnerTest::TearDown()
{
}

int TrikPyRunnerTest::run(const QString &script)
{
	QEventLoop l;
	QTimer::singleShot(5000, &l, std::bind(&QEventLoop::exit, &l, EXIT_TIMEOUT));
	QObject::connect(&*mScriptRunner, &trikScriptRunner::TrikScriptRunnerInterface::completed
					 , &l, [this, &l](const QString &e) {
		auto rc = EXIT_SCRIPT_SUCCESS;
		if (!e.isEmpty()) {
			rc = EXIT_SCRIPT_ERROR;
			std::cerr << qPrintable(e) << std::endl;
		}
		l.exit(rc);
	} );
	mStdOut.clear();
	mScriptRunner->run(script, "_.py");
	auto code = l.exec();
	std::cout << qPrintable(mStdOut) << std::endl;
	return code;
}

int TrikPyRunnerTest::runDirectCommandAndWaitForQuit(const QString &script)
{
	QEventLoop l;
	QObject::connect(&*mScriptRunner, &trikScriptRunner::TrikScriptRunnerInterface::completed
					 , &l, [&l](const QString &e) {
					l.exit(e.isEmpty() ? EXIT_SCRIPT_SUCCESS
									   : (qDebug() << e, EXIT_SCRIPT_ERROR));
	});
	mStdOut.clear();
	mScriptRunner->runDirectCommand(script);
	auto code = l.exec();
	QCoreApplication::sendPostedEvents();
	std::cout << mStdOut.toStdString() << std::endl;
	return code;
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

TEST_F(TrikPyRunnerTest, syntaxErrorReport)
{
	auto err = run("]");
	ASSERT_EQ(err, EXIT_SCRIPT_ERROR);
}

TEST_F(TrikPyRunnerTest, sanityCheck)
{
	auto err = run("1 + 1");
	ASSERT_EQ(err, EXIT_SCRIPT_SUCCESS);
	const auto &knownMethodNames = scriptRunner().knownMethodNames();
	ASSERT_TRUE(knownMethodNames.contains("brick"));
	ASSERT_TRUE(knownMethodNames.contains("setPower"));
	err = run("brick.motor('M2').setPower(10)");
	ASSERT_EQ(err, EXIT_SCRIPT_SUCCESS);
}

TEST_F(TrikPyRunnerTest, print)
{
	auto text = "Hello";
	auto err = runDirectCommandAndWaitForQuit(QString("print('") + text + "', end='')");
	ASSERT_EQ(err, EXIT_SCRIPT_SUCCESS);
	ASSERT_EQ(text, mStdOut.toStdString());
}

TEST_F(TrikPyRunnerTest, abortWhileTrue)
{
	QTimer t;
	t.setInterval(1000);
	t.setSingleShot(true);
	using trikScriptRunner::TrikScriptRunnerInterface;
	QObject::connect(&scriptRunner(), &TrikScriptRunnerInterface::startedScript
					 , &t, QOverload<>::of(&QTimer::start));
	QObject::connect(&t, &QTimer::timeout, &scriptRunner(), &TrikScriptRunnerInterface::abort);
	auto err = run("print('before')\nwhile True: pass\nprint('after')");
	ASSERT_EQ(mStdOut.toStdString(), "before\n");
	ASSERT_NE(err, EXIT_TIMEOUT);
	t.stop();
}

TEST_F(TrikPyRunnerTest, scriptWait)
{
	scriptRunner().run("script.wait(500)");
	tests::utils::Wait::wait(600);
}

TEST_F(TrikPyRunnerTest, directCommandContextWithTimersAndQtCore)
{
	auto err = runDirectCommandAndWaitForQuit("from PythonQt import QtCore as QtCore");
	ASSERT_EQ(err, EXIT_SCRIPT_SUCCESS);
	err = runDirectCommandAndWaitForQuit("import PythonQt");
	qDebug() << mStdOut;
	ASSERT_EQ(err, EXIT_SCRIPT_SUCCESS);
	err = runDirectCommandAndWaitForQuit("PythonQt.QTimer.singleShot(100, lambda _ : None)");
	ASSERT_EQ(err, EXIT_SCRIPT_SUCCESS);
	err = runDirectCommandAndWaitForQuit("t=QtCore.QTimer()");
	ASSERT_EQ(err, EXIT_SCRIPT_SUCCESS);
}

TEST_F(TrikPyRunnerTest, propertyAndMethodWithSimpleType)
{
	auto exitCode = run("brick.gyroscope().read()");
	ASSERT_EQ(exitCode, EXIT_SCRIPT_SUCCESS);
}

TEST_F(TrikPyRunnerTest, brickMethodWithNonTrivialReturnTypeConversion)
{
	auto exitCode = run("brick.getStillImage()");
	ASSERT_EQ(exitCode, EXIT_SCRIPT_SUCCESS);
}

TEST_F(TrikPyRunnerTest, brickPropertyAndVectorArgument)
{
	auto exitCode = run("brick.display().show([0], 1, 1, 'grayscale8')");
	ASSERT_EQ(exitCode, EXIT_SCRIPT_SUCCESS);
}

TEST_F(TrikPyRunnerTest, DISABLED_fileTestPy)
{
	auto err = runFromFile("file-test.py");
	ASSERT_EQ(err, EXIT_SCRIPT_SUCCESS);
}

TEST_F(TrikPyRunnerTest, scriptExecutionControl)
{
	auto exitCode = run("a = script.timer(1000)");
	ASSERT_EQ(exitCode, EXIT_SCRIPT_SUCCESS);
}

TEST_F(TrikPyRunnerTest, globalFunction)
{
	auto exitCode = run("script.getPhoto()");
	ASSERT_EQ(exitCode, EXIT_SCRIPT_SUCCESS);
}
