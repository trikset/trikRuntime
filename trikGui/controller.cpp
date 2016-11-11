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

#include "controller.h"

#include <QtCore/QProcess>
#include <QtCore/QFileInfo>

#include <QtXml/QDomDocument>

#include <trikKernel/configurer.h>
#include <trikKernel/fileUtils.h>
#include <trikKernel/paths.h>
#include <trikKernel/exceptions/internalErrorException.h>
#include <trikControl/brickFactory.h>
#include <trikNetwork/mailboxFactory.h>
#include <trikWiFi/trikWiFi.h>

#include "runningWidget.h"
#include "autoRunner.h"

using namespace trikGui;

const int communicatorPort = 8888;
const int telemetryPort = 9000;

Controller::Controller(const QString &configPath)
	: mBrick(trikControl::BrickFactory::create(configPath, trikKernel::Paths::mediaPath()))
{
	if (configPath.isEmpty()) {
		throw trikKernel::InternalErrorException("Config path is empty");
	}

	auto correctedConfigPath = configPath.endsWith('/') ? configPath : configPath + '/';

	trikKernel::Configurer configurer(correctedConfigPath + "system-config.xml"
			, correctedConfigPath + "model-config.xml");

	connect(mBrick->gamepad(), SIGNAL(disconnect()), this, SIGNAL(gamepadDisconnected()));
	connect(mBrick->gamepad(), SIGNAL(connected()), this, SIGNAL(gamepadConnected()));

	mMailbox.reset(trikNetwork::MailboxFactory::create(configurer));
	mTelemetry.reset(new trikTelemetry::TrikTelemetry(*mBrick));
	mScriptRunner.reset(new trikScriptRunner::TrikScriptRunner(*mBrick, mMailbox.data()));
	mCommunicator.reset(new trikCommunicator::TrikCommunicator(*mScriptRunner, configurer.version()));

	mWiFi.reset(new trikWiFi::TrikWiFi("/tmp/trikwifi", "/var/run/wpa_supplicant/wlan0", this));
	connect(mWiFi.data(), SIGNAL(connected()), this, SIGNAL(wiFiConnected()));
	connect(mWiFi.data(), SIGNAL(disconnected(trikWiFi::DisconnectReason)), this, SIGNAL(wiFiDisconnected()));

	connect(mCommunicator.data(), SIGNAL(stopCommandReceived()), this, SLOT(abortExecution()));
	connect(mCommunicator.data(), SIGNAL(connected()), this, SLOT(updateCommunicatorStatus()));
	connect(mCommunicator.data(), SIGNAL(disconnected()), this, SLOT(updateCommunicatorStatus()));
	connect(mTelemetry.data(), SIGNAL(connected()), this, SLOT(updateCommunicatorStatus()));
	connect(mTelemetry.data(), SIGNAL(disconnected()), this, SLOT(updateCommunicatorStatus()));
	connect(mMailbox.data(), SIGNAL(connectionStatusChanged(bool)), this, SIGNAL(mailboxStatusChanged(bool)));

	connect(mScriptRunner.data(), SIGNAL(completed(QString, int)), this, SLOT(scriptExecutionCompleted(QString, int)));

	connect(mScriptRunner.data(), SIGNAL(startedScript(QString, int))
			, this, SLOT(scriptExecutionFromFileStarted(QString, int)));

	connect(mScriptRunner.data(), SIGNAL(startedDirectScript(int))
			, this, SLOT(directScriptExecutionStarted(int)));

	connect(mBrick.data(), SIGNAL(stopped()), this, SIGNAL(brickStopped()));

	mCommunicator->startServer(communicatorPort);
	mTelemetry->startServer(telemetryPort);

	mAutoRunner.reset(new AutoRunner(*this));

	mBrick->led()->green();
}

Controller::~Controller()
{
	mBrick->led()->orange();
}

void Controller::runFile(const QString &filePath)
{
	const QFileInfo fileInfo(filePath);
	if (fileInfo.suffix() == "qts" || fileInfo.suffix() == "js") {
		mScriptRunner->run(trikKernel::FileUtils::readFromFile(fileInfo.canonicalFilePath()), fileInfo.baseName());
	} else if (fileInfo.suffix() == "wav" || fileInfo.suffix() == "mp3") {
		mScriptRunner->run("brick.playSound(\"" + fileInfo.canonicalFilePath() + "\");", fileInfo.baseName());
	} else if (fileInfo.suffix() == "sh") {
		QProcess::startDetached("sh", {filePath});
	} else if (fileInfo.suffix() == "exe") {
		QProcess::startDetached("mono", {filePath});
	} else if (fileInfo.suffix() == "py") {
		QProcess::startDetached("python", {filePath});
	} else if (fileInfo.isExecutable()) {
		QProcess::startDetached(filePath);
	}
}

void Controller::runScript(const QString &script)
{
	mScriptRunner->run(script);
}

void Controller::abortExecution()
{
	emit hideScriptWidgets();
	mScriptRunner->abort();

	// Now script engine will stop (after some time maybe) and send "completed" signal, which will be caught and
	// processed as if a script finished by itself.
}

trikControl::BrickInterface &Controller::brick()
{
	return *mBrick;
}

trikNetwork::MailboxInterface *Controller::mailbox()
{
	return mMailbox.data();
}

trikWiFi::TrikWiFi &Controller::wiFi()
{
	return *mWiFi;
}

bool Controller::communicatorConnectionStatus()
{
	return mTelemetry->activeConnections() > 0 && mCommunicator->activeConnections() > 0;
}

bool Controller::gamepadConnectionStatus() const
{
	if (mBrick->gamepad() != nullptr) {
		return mBrick->gamepad()->isConnected();
	} else {
		return false;
	}
}

void Controller::updateCommunicatorStatus()
{
	emit communicatorStatusChanged(communicatorConnectionStatus());
}

void Controller::scriptExecutionCompleted(const QString &error, int scriptId)
{
	if (error.isEmpty()) {
		emit hideRunningWidget(scriptId);
	} else {
		mCommunicator->sendMessage("error: " + error);
		emit showError(error, scriptId);
	}

	mBrick->reset();

	if (mMailbox) {
		mMailbox->clearQueue();
	}

	mBrick->led()->green();
}

void Controller::scriptExecutionFromFileStarted(const QString &fileName, int scriptId)
{
	emit showRunningWidget(fileName, scriptId);
}

void Controller::directScriptExecutionStarted(int scriptId)
{
	scriptExecutionFromFileStarted(tr("direct command"), scriptId);
}
