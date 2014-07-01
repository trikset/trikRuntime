/* Copyright 2013 Yurii Litvinov
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

#include "controller.h"

#include <QtCore/QProcess>
#include <QtCore/QFileInfo>
#include <QtCore/QDebug>

#include "runningWidget.h"

using namespace trikGui;

int const communicatorPort = 8888;

Controller::Controller(QString const &configPath, QString const &startDirPath)
	: mScriptRunner(configPath, startDirPath)
	, mCommunicator(mScriptRunner)
	, mRunningWidget(NULL)
	, mExecutionState(idle)
{
	connect(&mScriptRunner, SIGNAL(completed()), this, SLOT(scriptExecutionCompleted()));

	connect(&mCommunicator, SIGNAL(startedScript(QString)), this, SLOT(scriptExecutionFromFileStarted(QString)));
	connect(&mCommunicator, SIGNAL(startedDirectScript()), this, SLOT(directScriptExecutionStarted()));

	mCommunicator.startServer(communicatorPort);
}

Controller::~Controller()
{
	delete mRunningWidget;
}

void Controller::runFile(QString const &filePath)
{
	QFileInfo const fileInfo(filePath);
	if (fileInfo.suffix() == "qts" || fileInfo.suffix() == "js") {
		scriptExecutionFromFileStarted(fileInfo.baseName());
		mScriptRunner.runFromFile(fileInfo.canonicalFilePath());
	} else if (fileInfo.suffix() == "wav" || fileInfo.suffix() == "mp3") {
		mRunningWidget = new RunningWidget(fileInfo.baseName(), *this);
		mRunningWidget->show();
		mExecutionState = running;
		mScriptRunner.run("brick.playSound(\"" + fileInfo.canonicalFilePath() + "\");");
	} else if (fileInfo.suffix() == "sh") {
		QStringList args;
		args << filePath;
		QProcess::startDetached("sh", args);
	} else if (fileInfo.isExecutable()) {
		QProcess::startDetached(filePath);
	}
}

void Controller::abortExecution()
{
	mScriptRunner.abort();

	if (mExecutionState == running) {
		mExecutionState = stopping;
		mScriptRunner.run("brick.stop()");
	}
}

void Controller::scriptExecutionCompleted()
{
	if (mExecutionState == running) {
		mExecutionState = stopping;
		mScriptRunner.run("brick.stop()");
	} else {
		mExecutionState = idle;
		if (mRunningWidget) {
			mRunningWidget->releaseKeyboard();
			mRunningWidget->close();
			delete mRunningWidget;
			mRunningWidget = NULL;
		}
	}
}

void Controller::scriptExecutionFromFileStarted(QString const &fileName)
{
	if (mRunningWidget) {
		mRunningWidget->close();
		delete mRunningWidget;
	}

	mRunningWidget = new RunningWidget(fileName, *this);
	mRunningWidget->show();

	// After executing, a script will can open a widget for painting with trikControl::Display.
	// This widget will get all keyboard events and we won't be able to abort execution at Power
	// key press. So, mRunningWidget should grab the keyboard input. Nevertheless, the script
	// will can get keyboard events using trikControl::Keys class because it works directly
	// with the keyboard file.
	mRunningWidget->grabKeyboard();

	mExecutionState = running;
}

void Controller::directScriptExecutionStarted()
{
	scriptExecutionFromFileStarted(tr("direct command"));
}
