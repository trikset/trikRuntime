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
#include <trikNetwork/gamepadFactory.h>

#include "runningWidget.h"

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

	mGamepad.reset(trikNetwork::GamepadFactory::create(configurer));
	mMailbox.reset(trikNetwork::MailboxFactory::create(configurer));
	mTelemetry.reset(new trikTelemetry::TrikTelemetry(*mBrick, *mGamepad));

	mScriptRunner.reset(new trikScriptRunner::TrikScriptRunner(*mBrick, mMailbox.data(), mGamepad.data()));

	mCommunicator.reset(new trikCommunicator::TrikCommunicator(*mScriptRunner));

	connect(mCommunicator.data(), SIGNAL(stopCommandReceived()), this, SLOT(abortExecution()));

	connect(mScriptRunner.data(), SIGNAL(completed(QString, int)), this, SLOT(scriptExecutionCompleted(QString, int)));

	connect(mScriptRunner.data(), SIGNAL(startedScript(QString, int))
			, this, SLOT(scriptExecutionFromFileStarted(QString, int)));

	connect(mScriptRunner.data(), SIGNAL(startedDirectScript(int))
			, this, SLOT(directScriptExecutionStarted(int)));

	connect(mBrick.data(), SIGNAL(stopped()), this, SIGNAL(brickStopped()));

	mCommunicator->startServer(communicatorPort);
	mTelemetry->startServer(telemetryPort);
}

Controller::~Controller()
{
}

void Controller::runFile(const QString &filePath)
{
	QFileInfo const fileInfo(filePath);
	if (fileInfo.suffix() == "qts" || fileInfo.suffix() == "js") {
		mScriptRunner->run(trikKernel::FileUtils::readFromFile(fileInfo.canonicalFilePath()), fileInfo.baseName());
	} else if (fileInfo.suffix() == "wav" || fileInfo.suffix() == "mp3") {
		mScriptRunner->run("brick.playSound(\"" + fileInfo.canonicalFilePath() + "\");", fileInfo.baseName());
	} else if (fileInfo.suffix() == "sh") {
		QProcess::startDetached("sh", {filePath});
	} else if (fileInfo.isExecutable()) {
		QProcess::startDetached(filePath);
	}
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

void Controller::scriptExecutionCompleted(const QString &error, int scriptId)
{
	if (error.isEmpty()) {
		emit hideRunningWidget(scriptId);
	} else {
		mCommunicator->sendMessage("error: " + error);
		emit showError(error, scriptId);
	}
}

void Controller::scriptExecutionFromFileStarted(const QString &fileName, int scriptId)
{
	emit showRunningWidget(fileName, scriptId);
}

void Controller::directScriptExecutionStarted(int scriptId)
{
	scriptExecutionFromFileStarted(tr("direct command"), scriptId);
}
