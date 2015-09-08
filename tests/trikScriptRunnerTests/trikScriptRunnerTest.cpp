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
#include <QtCore/QEventLoop>
#include <QtCore/QFile>
#include <QtCore/QTimer>

#include <QtScript/QScriptContext>
#include <QtScript/QScriptEngine>
#include <QtScript/QScriptValue>

#include <trikControl/brickFactory.h>

#include <trikKernel/fileUtils.h>

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
	mScriptRunner.reset(new trikScriptRunner::TrikScriptRunner(*mBrick, nullptr, nullptr));
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

void TrikScriptRunnerTest::runFromFile(const QString &fileName)
{
	const auto fileContents = trikKernel::FileUtils::readFromFile("data/" + fileName);
	run(fileContents);
}

void TrikScriptRunnerTest::wait(int msec)
{
	QEventLoop waitingLoop;
	QTimer::singleShot(msec, &waitingLoop, SLOT(quit()));
	waitingLoop.exec();
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
	wait(2100);
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
