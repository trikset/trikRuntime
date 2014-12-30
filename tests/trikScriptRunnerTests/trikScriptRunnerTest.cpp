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
