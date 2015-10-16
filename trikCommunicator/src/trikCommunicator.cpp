/* Copyright 2013 - 2015 Yurii Litvinov, CyberTech Labs Ltd.
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

#include "trikCommunicator.h"

#include <trikScriptRunner/trikScriptRunner.h>

#include "src/connection.h"

using namespace trikCommunicator;

TrikCommunicator::TrikCommunicator(trikControl::BrickInterface &brick
		, trikNetwork::MailboxInterface * const mailbox
		, trikNetwork::GamepadInterface * const gamepad
		)
	: TrikCommunicator(new trikScriptRunner::TrikScriptRunner(brick, mailbox, gamepad), true)
{
}

TrikCommunicator::TrikCommunicator(trikScriptRunner::TrikScriptRunner &runner)
	: TrikCommunicator(&runner, false)
{
}

TrikCommunicator::TrikCommunicator(trikScriptRunner::TrikScriptRunner * const runner, bool hasScriptRunnerOwnership)
	: trikNetwork::TrikServer([this] () { return connectionFactory(); })
	, mTrikScriptRunner(runner)
	, mHasScriptRunnerOwnership(hasScriptRunnerOwnership)
{
	setObjectName("TrikCommunicator");
	qRegisterMetaType<trikScriptRunner::TrikScriptRunner *>("trikScriptRunner::TrikScriptRunner *");

	connect(runner, SIGNAL(sendMessage(QString)), this, SLOT(sendPrintMessage(QString)));
}

TrikCommunicator::~TrikCommunicator()
{
	if (mHasScriptRunnerOwnership) {
		delete mTrikScriptRunner;
	}
}

Connection *TrikCommunicator::connectionFactory()
{
	Connection *connection = new Connection(*mTrikScriptRunner);
	connect(connection, SIGNAL(stopCommandReceived()), this, SIGNAL(stopCommandReceived()));
	return connection;
}

void TrikCommunicator::sendPrintMessage(const QString &text)
{
	sendMessage("print: " + text);
}
