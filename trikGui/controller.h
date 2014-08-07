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

#pragma once

#include <trikCommunicator/trikCommunicator.h>
#include <trikScriptRunner/trikScriptRunner.h>
#include <trikControl/brick.h>

namespace trikGui
{

class RunningWidget;

/// Provides backend features like Qt Script runner for frontend.
class Controller : public QObject
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param configPath - path to config file for trikControl, for example, /home/root/trik/.
	/// @param startDirPath - path to the directory from which the application was executed.
	Controller(QString const &configPath, QString const &startDirPath);

	~Controller();

	/// Executes specified file as Qt Script, if it has .qts extension, or as a program otherwise.
	void runFile(QString const &filePath);

	/// Cancels execution of current program.
	void abortExecution();

	trikControl::Brick &brick();

private slots:
	void scriptExecutionCompleted();

	void scriptExecutionFromFileStarted(QString const &fileName);
	void directScriptExecutionStarted();

private:
	trikScriptRunner::TrikScriptRunner mScriptRunner;
	trikCommunicator::TrikCommunicator mCommunicator;
	trikControl::Brick mBrick;

	RunningWidget * mRunningWidget;  // Has ownership.
};

}
