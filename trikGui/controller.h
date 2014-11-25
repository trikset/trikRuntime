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
#include <trikTelemetry/trikTelemetry.h>
#include <trikControl/brick.h>
#include "mainWidget.h"

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

	/// Returns reference to Brick object, which provides access to low-level robot functionality.
	trikControl::Brick &brick();

	/// Returns mStartDirPath (path to the directory from which the application was executed)
	QString startDirPath() const;

	/// Returns path to the directory in which scripts must be saved
	QString scriptsDirPath() const;

	/// Returns name of the directory in which scripts must be saved
	QString scriptsDirName() const;

signals:
	/// Emitted when controller created a runningWidget (for scripts) and wants it to be shown.
	void addRunningWidget(MainWidget &widget);

	/// Emitted when controller removed the runningWidget and wants it to be closed.
	void closeRunningWidget(MainWidget &widget);

private slots:
	void scriptExecutionCompleted(QString const &error, int scriptId);

	void scriptExecutionFromFileStarted(QString const &fileName, int scriptId);
	void directScriptExecutionStarted(int scriptId);

private:
	trikControl::Brick mBrick;
	trikScriptRunner::TrikScriptRunner mScriptRunner;
	trikCommunicator::TrikCommunicator mCommunicator;
	trikTelemetry::TrikTelemetry mTelemetry;

	QHash<int, RunningWidget *> mRunningWidgets;  // Has ownership.
	QString const &mStartDirPath; // Path to the directory from which the application was executed
};

}
