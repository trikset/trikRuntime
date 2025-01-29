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
#include <trikControl/gamepadInterface.h>
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

	connect(mBrick->gamepad(), &trikControl::GamepadInterface::disconnected
			, this, &Controller::gamepadDisconnected);
	connect(mBrick->gamepad(), &trikControl::GamepadInterface::connected
			, this, &Controller::gamepadConnected);

	mMailbox.reset(trikNetwork::MailboxFactory::create(configurer));
	mTelemetry.reset(new trikTelemetry::TrikTelemetry(*mBrick));
	mScriptRunner.reset(new trikScriptRunner::TrikScriptRunner(*mBrick, mMailbox.data()));
	mCommunicator.reset(new trikCommunicator::TrikCommunicator(*mScriptRunner, configurer.version()));

	mWiFi.reset(new trikWiFi::TrikWiFi("/tmp/trikwifi", "/var/run/wpa_supplicant/wlan0", this));
	connect(mWiFi.data(), &trikWiFi::TrikWiFi::connected, this, &Controller::wiFiConnected);
	connect(mWiFi.data(), &trikWiFi::TrikWiFi::disconnected, this, &Controller::wiFiDisconnected);

	connect(mCommunicator.data(), &trikCommunicator::TrikCommunicator::stopCommandReceived
			, this, &Controller::abortExecution);
	connect(mCommunicator.data(), &trikCommunicator::TrikCommunicator::connected
			, this, &Controller::updateCommunicatorStatus);
	connect(mCommunicator.data(), &trikCommunicator::TrikCommunicator::disconnected
			, this, &Controller::updateCommunicatorStatus);
	connect(mTelemetry.data(), &trikTelemetry::TrikTelemetry::connected
			, this, &Controller::updateCommunicatorStatus);
	connect(mTelemetry.data(), &trikTelemetry::TrikTelemetry::disconnected
			, this, &Controller::updateCommunicatorStatus);
	connect(mMailbox.data(), &trikNetwork::MailboxInterface::connectionStatusChanged
			, this, &Controller::mailboxStatusChanged);

	connect(mScriptRunner.data(), &trikScriptRunner::TrikScriptRunner::completed
			, this, &Controller::scriptExecutionCompleted);

	connect(mScriptRunner.data(), &trikScriptRunner::TrikScriptRunner::textInStdOut
		, mScriptRunner.data(), [](const QString &m){
		QTextStream(stdout) << m;
	});

	connect(mScriptRunner.data(), &trikScriptRunner::TrikScriptRunner::startedScript
			, this, &Controller::scriptExecutionFromFileStarted);

	connect(mScriptRunner.data(), &trikScriptRunner::TrikScriptRunner::startedDirectScript
			, this, &Controller::directScriptExecutionStarted);

	connect(mBrick.data(), &trikControl::BrickInterface::stopped, this, &Controller::brickStopped);

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
		mScriptRunner->run(trikKernel::FileUtils::readFromFile(fileInfo.canonicalFilePath()),
						   trikScriptRunner::ScriptType::PYTHON, fileInfo.baseName());
	} else if (fileInfo.isExecutable()) {
		QProcess::startDetached(filePath, {});
	}
}

void Controller::runScript(const QString &script)
{
	mScriptRunner->run(script);
}

void Controller::abortExecution()
{
	Q_EMIT hideScriptWidgets();
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
	Q_EMIT communicatorStatusChanged(communicatorConnectionStatus());
}

void Controller::scriptExecutionCompleted(const QString &error, int scriptId)
{
	if (error.isEmpty()) {
		Q_EMIT hideRunningWidget(scriptId);
	} else {
		mCommunicator->sendMessage("error: " + error);
		Q_EMIT showError(error, scriptId);
	}

	mBrick->reset();

	if (mMailbox) {
		mMailbox->clearQueue();
	}

	mBrick->led()->green();
}

void Controller::scriptExecutionFromFileStarted(const QString &fileName, int scriptId)
{
	Q_EMIT showRunningWidget(fileName, scriptId);
}

void Controller::directScriptExecutionStarted(int scriptId)
{
	scriptExecutionFromFileStarted(tr("direct command"), scriptId);
}
