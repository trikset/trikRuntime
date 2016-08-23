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
#include <trikNetwork/mailboxInterface.h>
#include <trikScriptRunner/trikScriptRunner.h>
#include <trikTelemetry/trikTelemetry.h>

#include "lazyMainWidget.h"

namespace trikWiFi {
class TrikWiFi;
}

namespace trikGui {

class RunningWidget;
class AutoRunner;

/// Provides backend features like Qt Script runner for frontend.
class Controller : public QObject
{
	Q_OBJECT

public:

	/// Constructor.
	/// @param configPath - path to config file for trikControl, for example, /home/root/trik/.
	explicit Controller(const QString &configPath);

	~Controller() override;

	/// Executes specified file as Qt Script, if it has .qts extension, or as a program otherwise.
	void runFile(const QString &filePath);

	/// Executes given script.
	void runScript(const QString &script);

	/// Returns reference to Brick object, which provides access to robot hardware.
	trikControl::BrickInterface &brick();

	/// Returns reference to Mailbox object, which provides access to robot-to-robot communications.
	/// Does not pass ownership to the caller.
	trikNetwork::MailboxInterface *mailbox();

	/// Reference to WiFi manager.
	trikWiFi::TrikWiFi &wiFi();

	/// Asks controller to correctly close given running widget.
	void doCloseRunningWidget(MainWidget &widget);

	/// Returns communicator connection status (whether or not both Telemetry and Communicator servers are connected).
	bool communicatorConnectionStatus();

	/// Returns gamepad connection status.
	bool gamepadConnectionStatus() const;

public slots:
	/// Cancels execution of current program.
	void abortExecution();

signals:
	/// Emitted when a new script starts and therefore a running widget must be shown with the script's name.
	void showRunningWidget(const QString &fileName, int scriptId);

	/// Emitted when running widget for a script with a given id must be closed.
	void hideRunningWidget(int scriptId);

	/// Emitted when a script stops due to an error.
	void showError(const QString &error, int scriptId);

	/// Emitted when a graphics widget must be closed.
	void hideGraphicsWidget();

	/// Emitted when both graphics and running widget must be closed.
	void hideScriptWidgets();

	/// Emitted when brick has finished deferred deinitialization so we need to refresh display to clear possible
	/// clutter from videosensors.
	void brickStopped();

	/// Emitted when a robot is disconnected from a gamepad.
	void gamepadDisconnected();

	/// Emitted when a robot is connected to a gamepad.
	void gamepadConnected();

	/// Emitted when a robot is connected to a network.
	void wiFiConnected();

	/// Emitted when a robot is disconnected from a network.
	void wiFiDisconnected();

	/// Emitted when Telemetry and Communicator servers' status changes.
	/// @param status - true if both servers are connected.
	void communicatorStatusChanged(bool status);

	/// Emitted when Mailbox server's status changes.
	void mailboxStatusChanged(bool connected);

private slots:
	void scriptExecutionCompleted(const QString &error, int scriptId);

	void scriptExecutionFromFileStarted(const QString &fileName, int scriptId);
	void directScriptExecutionStarted(int scriptId);

	void updateCommunicatorStatus();

private:
	QScopedPointer<trikControl::BrickInterface> mBrick;
	QScopedPointer<trikNetwork::MailboxInterface> mMailbox;
	QScopedPointer<trikScriptRunner::TrikScriptRunner> mScriptRunner;
	QScopedPointer<trikCommunicator::TrikCommunicator> mCommunicator;
	QScopedPointer<trikTelemetry::TrikTelemetry> mTelemetry;
	QScopedPointer<trikWiFi::TrikWiFi> mWiFi;
	QScopedPointer<AutoRunner> mAutoRunner;

	QHash<int, RunningWidget *> mRunningWidgets;  // Has ownership.
};

}
