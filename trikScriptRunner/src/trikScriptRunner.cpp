/* Copyright 2014 - 2015 CyberTech Labs Ltd.
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

#include <QsLog.h>

#include "trikScriptRunner.h"
#include "trikJavaScriptRunner.h"
#include "trikPythonRunner.h"

using namespace trikScriptRunner;

/// Constructor.
/// @param brick - reference to trikControl::Brick instance.
/// @param mailbox - mailbox object used to communicate with other robots.
TrikScriptRunner::TrikScriptRunner(trikControl::BrickInterface &brick
								   , trikNetwork::MailboxInterface * const mailbox
								   )
	: brick(brick), mailbox(mailbox), mLastRunner(JAVASCRIPT)
{}

TrikScriptRunner::~TrikScriptRunner()
{
	for (size_t i = 0; i < ScriptTypeLength; i++) { // stop and delete all working interpretators
		if (mScriptRunnerArray[i] != nullptr) {
			mScriptRunnerArray[i]->abort();
			delete mScriptRunnerArray[i];
		}
	}
}

void TrikScriptRunner::registerUserFunction(const QString &name, QScriptEngine::FunctionSignature function)
{
	fetchRunner(mLastRunner)->registerUserFunction(name, function);
}

void TrikScriptRunner::addCustomEngineInitStep(const std::function<void (QScriptEngine *)> &step)
{
	fetchRunner(mLastRunner)->addCustomEngineInitStep(step);
}

QStringList TrikScriptRunner::knownMethodNames() const
{
	return mScriptRunnerArray[mLastRunner]->knownMethodNames();
}

void TrikScriptRunner::run(const QString &script, const QString &fileName)
{
	if (fileName.endsWith(".py")) {
		run(script, PYTHON, fileName);
	} else { // default JS
		run(script, JAVASCRIPT, fileName);
	}
}

TrikScriptRunnerInterface * TrikScriptRunner::fetchRunner(const ScriptType &stype)
{
	if (mScriptRunnerArray[stype] == nullptr) { // lazy creation
		switch (stype) {
			case JAVASCRIPT:
				mScriptRunnerArray[stype] = new TrikJavaScriptRunner(brick, mailbox);
				break;
			case PYTHON:
				mScriptRunnerArray[stype] = new TrikPythonRunner(brick, mailbox);
				break;
			default:
				QLOG_ERROR() << "Can't handle script with unrecognized type: " << stype;
				return nullptr;
		}
		// subscribe on wrapped objects signals
		connect(mScriptRunnerArray[stype], SIGNAL(completed(QString, int)),
				this, SIGNAL(completed(QString, int)));
		connect(mScriptRunnerArray[stype], SIGNAL(startedScript(const QString &, int)),
				this, SIGNAL(startedScript(const QString &, int)));
		connect(mScriptRunnerArray[stype], SIGNAL(startedDirectScript(int)),
				this, SIGNAL(startedDirectScript(int)));
		connect(mScriptRunnerArray[stype], SIGNAL(sendMessage(const QString &)),
				this, SIGNAL(sendMessage(const QString &)));
	}

	mLastRunner = stype;

	return mScriptRunnerArray[stype];
}

void TrikScriptRunner::run(const QString &script, const ScriptType &stype, const QString &fileName)
{
	abortAll(); // FIXME: or fetchRunner(stype)->abort()? or abort(/*last*/)?

	fetchRunner(stype)->run(script, fileName);
}

void TrikScriptRunner::runDirectCommand(const QString &command)
{
	fetchRunner(mLastRunner)->runDirectCommand(command);
}

void TrikScriptRunner::abort()
{
	fetchRunner(mLastRunner)->abort();
}

void TrikScriptRunner::abortAll()
{
	for (size_t i = 0; i < ScriptTypeLength; i++) {
		if (mScriptRunnerArray[i] != nullptr) {
			mScriptRunnerArray[i]->abort();
		}
	}
}

void TrikScriptRunner::brickBeep()
{
	fetchRunner(mLastRunner)->brickBeep();
}
