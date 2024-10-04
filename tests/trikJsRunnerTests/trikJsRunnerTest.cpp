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
#include <QRandomGenerator>
#include <QCoreApplication>
#include <QTimer>
#include <QOperatingSystemVersion>


using namespace tests;
constexpr auto SCRIPT_EXECUTION_TIMEOUT = 10000;
constexpr auto EXIT_TIMEOUT = -93;
constexpr auto EXIT_SCRIPT_ERROR = 113;
constexpr auto EXIT_SCRIPT_SUCCESS = EXIT_SUCCESS;

QScriptValue scriptAssert(QScriptContext *context, QScriptEngine *engine)
{
	Q_UNUSED(engine)

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
					, "./test-model-config.xml", "./media"));
	mScriptRunner.reset(new trikScriptRunner::TrikScriptRunner(*mBrick, nullptr));
	mScriptRunner->registerUserFunction("assert", scriptAssert);
	QObject::connect(mScriptRunner.data(), &trikScriptRunner::TrikScriptRunnerInterface::textInStdOut,
					 mScriptRunner.data(), [this](const QString &m) { mStdOut += m; });
}

void TrikJsRunnerTest::TearDown()
{
	// Cleanup in proper order
	mScriptRunner.reset();
	mBrick.reset();
}

int TrikJsRunnerTest::run(const QString &script, const QString &file)
{
	QEventLoop wait;
	QObject::connect(mScriptRunner.data(), &trikScriptRunner::TrikScriptRunner::completed
					 , &wait, [&wait](QString error, int)
	{
		if (!error.isEmpty()) {
			std::cout << "Engine returned error:" << error.toStdString() << std::endl;
		}
		wait.exit(error.isEmpty() ? EXIT_SCRIPT_SUCCESS : EXIT_SCRIPT_ERROR);
	}, Qt::QueuedConnection ) ; // prevent `exit` before `exec` via QueuedConnection
	QTimer::singleShot(SCRIPT_EXECUTION_TIMEOUT, &wait, std::bind(&QEventLoop::exit, &wait, EXIT_TIMEOUT));

	mScriptRunner->run(script, file);
	auto exitCode = wait.exec();
	if (!mStdOut.isEmpty())
		std::cout << "stdout:" << mStdOut.toStdString() << std::endl;
	return exitCode;
}

int TrikJsRunnerTest::runDirectCommandAndWaitForQuit(const QString &script)
{
	QEventLoop wait;
	QObject::connect(mScriptRunner.data(), &trikScriptRunner::TrikScriptRunner::completed
					 , &wait,  [&wait](QString error, int)
	{
		if (!error.isEmpty()) {
			std::cout << "Engine returned error:" << error.toStdString() << std::endl;
		}
		wait.exit(error.isEmpty() ? EXIT_SCRIPT_SUCCESS : EXIT_SCRIPT_ERROR);
	} , Qt::QueuedConnection ) ; // prevent `exit` before `exec` via QueuedConnection
	QTimer::singleShot(SCRIPT_EXECUTION_TIMEOUT, &wait, std::bind(&QEventLoop::exit, &wait, EXIT_TIMEOUT));
	mScriptRunner->runDirectCommand(script);

	auto exitCode = wait.exec();
	if (!mStdOut.isEmpty())
		std::cout << "stdout:" << mStdOut.toStdString() << std::endl;
	return exitCode;
}

int TrikJsRunnerTest::runFromFile(const QString &fileName)
{
	try {
		QString fileContents = trikKernel::FileUtils::readFromFile(":/data/"+fileName);

		if (QOperatingSystemVersion::currentType() == QOperatingSystemVersion::OSType::Windows) {
			fileContents = fileContents.replace("&&", ";");
		}
		return run(fileContents, fileName);
	} catch (trikKernel::TrikRuntimeException &e) {
		std::cout << e.what() << std::endl;
		return EXIT_SCRIPT_ERROR;
	}
}

trikScriptRunner::TrikScriptRunner &TrikJsRunnerTest::scriptRunner()
{
	return *mScriptRunner;
}

TEST_F(TrikJsRunnerTest, sanityCheck)
{
	auto errCode = run("1", "_.js");
	ASSERT_EQ(errCode, EXIT_SCRIPT_SUCCESS);
	errCode = run("1 + 1", "_.js");
	ASSERT_EQ(errCode, EXIT_SCRIPT_SUCCESS);
}

TEST_F(TrikJsRunnerTest, syntaxErrorCheck)
{
	auto errCode = run("/1", "_.js");
	ASSERT_EQ(errCode, EXIT_SCRIPT_ERROR);
}

TEST_F(TrikJsRunnerTest, scriptWaitQuit)
{
	const QString test = "s = Date.now();"
				   "timeout=%1;"
				   "script.wait(timeout);"
				   "e = Date.now();"
				   "print('Elapsed ', e-s, ' ms with expected ', timeout, ' ms');"
				   "/*assert(Math.abs(e-s-timeout) <= Math.max(2, timeout/111));*/"
				   "script.quit();";

	for (auto &&t: { 1000, 500, 200, 100, 50, 20, 10, 5, 3, 0}) {
		auto err = runDirectCommandAndWaitForQuit(test.arg(t));
		EXPECT_EQ(err, EXIT_SCRIPT_SUCCESS);
		mStdOut.clear();
	}
}

TEST_F(TrikJsRunnerTest, twoProgramsTest)
{
	scriptRunner().run("script.wait(500);");
	tests::utils::Wait::wait(100);
	scriptRunner().run("script.wait(500);");
	tests::utils::Wait::wait(600);
}

TEST_F(TrikJsRunnerTest, printTest)
{
	const QString text = "Hello";
	auto err = runDirectCommandAndWaitForQuit("print('" + text + "');script.quit();");
	ASSERT_EQ(err, EXIT_SCRIPT_SUCCESS);
	ASSERT_EQ(text + '\n', mStdOut);
}

TEST_F(TrikJsRunnerTest, brickInterfaceAccess)
{
	const auto &knownMethodNames = scriptRunner().knownMethodNames();
	ASSERT_TRUE(knownMethodNames.contains("brick"));
	ASSERT_TRUE(knownMethodNames.contains("setPower"));
	auto errCode = run("brick.sensor(A1).read()", "_.js");
	ASSERT_EQ(errCode, EXIT_SCRIPT_SUCCESS);
}

TEST_F(TrikJsRunnerTest, fileTestJs)
{
	auto errCode = runFromFile("file-test.js");
	ASSERT_EQ(errCode, EXIT_SCRIPT_SUCCESS);
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
	runDirectCommandAndWaitForQuit("script.system('echo 123 > test', true);");
	ASSERT_TRUE(testFile.exists());

	runDirectCommandAndWaitForQuit("script.system('"
#ifdef Q_OS_WIN
"DEL"
#else
"rm"
#endif
	" test', true);");
	ASSERT_FALSE(testFile.exists());
	runDirectCommandAndWaitForQuit("script.quit();");
}

TEST_F(TrikJsRunnerTest, directCommandThatQuitsImmediatelyTest)
{
	auto testFileName = "test" + QString::number(QRandomGenerator::global()->generate(), 16);
	::remove(testFileName.toStdString().c_str());
	ASSERT_FALSE(QFileInfo::exists(testFileName));
	auto exitCode = runDirectCommandAndWaitForQuit("script.system('echo 123 > "
												   + testFileName + "', true); script.quit();");
	ASSERT_EQ(exitCode, EXIT_SCRIPT_SUCCESS);
	ASSERT_TRUE(QFileInfo::exists(testFileName));
	tests::utils::Wait::wait(300);
	runDirectCommandAndWaitForQuit("script.system('"
#ifdef Q_OS_WIN
	"DEL"
#else
	"rm"
#endif
	" " + testFileName + "', true); script.quit();");
	ASSERT_FALSE(QFileInfo::exists(testFileName));
}
