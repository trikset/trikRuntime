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

#include <trikKernel/fileUtils.h>

#include "runningWidget.h"

using namespace trikGui;

int const communicatorPort = 8888;

Controller::Controller(QString const &configPath, QString const &startDirPath)
	: mBrick(*thread(), configPath, startDirPath)
	, mScriptRunner(mBrick, startDirPath)
	, mCommunicator(mScriptRunner)
	, mRunningWidget(NULL)
	, mStartDirPath(startDirPath)
{
	connect(&mScriptRunner, SIGNAL(completed(QString)), this, SLOT(scriptExecutionCompleted(QString)));

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
		mScriptRunner.run(trikKernel::FileUtils::readFromFile(fileInfo.canonicalFilePath()));
	} else if (fileInfo.suffix() == "wav" || fileInfo.suffix() == "mp3") {
		mRunningWidget = new RunningWidget(fileInfo.baseName(), *this);
		mRunningWidget->show();
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

	// Now script engine will stop (after some time maybe) and send "completed" signal, which will be caught and
	// processed as if a script finished by itself.
}

trikControl::Brick &Controller::brick()
{
	return mBrick;
}

QString Controller::startDirPath()
{
	return mStartDirPath;
}

QString Controller::scriptsDirPath()
{
	return mScriptRunner.scriptsDirPath();
}

QString Controller::scriptsDirName()
{
	return mScriptRunner.scriptsDirName();
}

void Controller::scriptExecutionCompleted(QString const &error)
{
	if (mRunningWidget && error.isEmpty()) {
		mRunningWidget->releaseKeyboard();
		mRunningWidget->close();

		// Here we can be inside handler of mRunningWidget key press event.
		mRunningWidget->deleteLater();
		mRunningWidget = NULL;
	} else if (!error.isEmpty()) {
		mRunningWidget->showError(error);
		mCommunicator.sendMessage("error: " + error);
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

	// After executing, a script will open a widget for painting with trikControl::Display.
	// This widget will get all keyboard events and we won't be able to abort execution at Power
	// key press. So, mRunningWidget should grab the keyboard input. Nevertheless, the script
	// can get keyboard events using trikControl::Keys class because it works directly
	// with the keyboard file.
	mRunningWidget->grabKeyboard();
}

void Controller::directScriptExecutionStarted()
{
	scriptExecutionFromFileStarted(tr("direct command"));
}
