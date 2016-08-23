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

#include <trikControl/brickInterface.h>

#include "src/connection.h"

using namespace trikCommunicator;
using namespace trikKernel;

TrikCommunicator::TrikCommunicator(trikControl::BrickInterface &brick
		, trikNetwork::MailboxInterface * const mailbox
		)
	: TrikCommunicator(createDifferentOwnerPointer(new trikScriptRunner::TrikScriptRunner(brick, mailbox))
			, brick.configVersion())
{
}

TrikCommunicator::TrikCommunicator(trikScriptRunner::TrikScriptRunner &runner, const QString &configVersion)
	: TrikCommunicator(createDifferentOwnerPointer(runner), configVersion)
{
}

TrikCommunicator::TrikCommunicator(const trikKernel::DifferentOwnerPointer<trikScriptRunner::TrikScriptRunner> &runner
		, const QString &configVersion)
	: trikNetwork::TrikServer([this] () { return connectionFactory(); })
	, mTrikScriptRunner(runner)
	, mConfigVersion(configVersion)
{
	setObjectName("TrikCommunicator");
	qRegisterMetaType<trikScriptRunner::TrikScriptRunner *>("trikScriptRunner::TrikScriptRunner *");

	connect(runner.data(), SIGNAL(sendMessage(QString)), this, SLOT(sendMessage(QString)));
}

TrikCommunicator::~TrikCommunicator()
{
}

Connection *TrikCommunicator::connectionFactory()
{
	Connection * const connection = new Connection(*mTrikScriptRunner, mConfigVersion);
	connect(connection, SIGNAL(stopCommandReceived()), this, SIGNAL(stopCommandReceived()));
	return connection;
}
