/* Copyright 2013 - 2014 Yurii Litvinov, CyberTech Labs Ltd.
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

#include <QtCore/QFile>
#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

#include <trikScriptRunner/trikScriptRunner.h>

#include "src/connection.h"

using namespace trikCommunicator;

TrikCommunicator::TrikCommunicator(trikControl::BrickInterface &brick, QString const &startDirPath)
	: TrikCommunicator(new trikScriptRunner::TrikScriptRunner(brick, startDirPath), true)
{
}

TrikCommunicator::TrikCommunicator(trikScriptRunner::TrikScriptRunner &runner)
	: TrikCommunicator(&runner, false)
{
}

TrikCommunicator::TrikCommunicator(trikScriptRunner::TrikScriptRunner * const runner, bool hasScriptRunnerOwnership)
	: trikKernel::TrikServer([this] () { return connectionFactory(); })
	, mTrikScriptRunner(runner)
	, mHasScriptRunnerOwnership(hasScriptRunnerOwnership)
{
	qRegisterMetaType<trikScriptRunner::TrikScriptRunner *>("trikScriptRunner::TrikScriptRunner *");

	connect(mTrikScriptRunner, SIGNAL(completed(QString, int)), this, SIGNAL(finishedScript()));
}

TrikCommunicator::~TrikCommunicator()
{
	if (mHasScriptRunnerOwnership) {
		delete mTrikScriptRunner;
	}
}

Connection *TrikCommunicator::connectionFactory()
{
	auto connection = new Connection(*mTrikScriptRunner);
	return connection;
}
