/* Copyright 2013 - 2015 Yurii Litvinov and CyberTech Labs Ltd.
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
#include <trikControl/brickInterface.h>
#include <trikKernel/lazyMainWidget.h>
#include <trikNetwork/gamepadInterface.h>
#include <trikNetwork/mailboxInterface.h>
#include <trikScriptRunner/trikScriptRunner.h>
#include <trikTelemetry/trikTelemetry.h>

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

	/// Returns reference to Brick object, which provides access to robot hardware.
	trikControl::BrickInterface &brick();

	/// Returns reference to Mailbox object, which provides access to robot-to-robot communications.
	trikNetwork::MailboxInterface &mailbox();

	/// Returns mStartDirPath (path to the directory from which the application was executed)
	QString startDirPath() const;

	/// Returns path to the directory in which scripts must be saved
	QString scriptsDirPath() const;

	/// Returns name of the directory in which scripts must be saved
	QString scriptsDirName() const;

	/// Asks controller to correctly close given running widget.
	void doCloseRunningWidget(trikKernel::MainWidget &widget);

signals:
	/// Emitted when controller created a runningWidget (for scripts) and wants it to be shown.
	void addRunningWidget(trikKernel::MainWidget &widget);

	/// Emitted when controller removed the runningWidget and wants it to be closed.
	void closeRunningWidget(trikKernel::MainWidget &widget);

	/// Emitted when controller wants GraphicsWidget to be shown.
	void addGraphicsWidget(trikKernel::LazyMainWidget &widget);

	/// Emitted when controller wants GraphicsWidget to be closed.
	void closeGraphicsWidget(trikKernel::MainWidget &widget);

	/// Emitted when brick has finished deferred deinitialization so we need to refresh display to clear possible
	/// clutter from videosensors.
	void brickStopped();

private slots:
	void scriptExecutionCompleted(QString const &error, int scriptId);

	void scriptExecutionFromFileStarted(QString const &fileName, int scriptId);
	void directScriptExecutionStarted(int scriptId);

private:
	QScopedPointer<trikControl::BrickInterface> mBrick;
	QScopedPointer<trikNetwork::GamepadInterface> mGamepad;
	QScopedPointer<trikNetwork::MailboxInterface> mMailbox;
	QScopedPointer<trikScriptRunner::TrikScriptRunner> mScriptRunner;
	QScopedPointer<trikCommunicator::TrikCommunicator> mCommunicator;
	QScopedPointer<trikTelemetry::TrikTelemetry> mTelemetry;

	QHash<int, RunningWidget *> mRunningWidgets;  // Has ownership.
	QString const &mStartDirPath; // Path to the directory from which the application was executed
};

}
