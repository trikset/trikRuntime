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
#include <QTimer>
#include "trikScriptRunner.h"
#include "trikJavaScriptRunner.h"
#include "trikPythonRunner.h"
#include "trikCppRunner.h"

#include <trikKernel/timeVal.h>
#include "threading.h"
#include <QCoreApplication>
#include <QMetaMethod>
#include <QStack>

using namespace trikControl;
using namespace trikScriptRunner;
using namespace trikNetwork;
using namespace trikKernel;

Q_DECLARE_METATYPE(QVector<uint8_t>)
Q_DECLARE_METATYPE(QVector<int>)
Q_DECLARE_METATYPE(QTimer*)

/// Constructor.
/// @param brick - reference to trikControl::Brick instance.
/// @param mailbox - mailbox object used to communicate with other robots.
TrikScriptRunner::TrikScriptRunner(trikControl::BrickInterface &brick
								   , trikNetwork::MailboxInterface * const mailbox
								   , TrikScriptControlInterface * scriptControl
								   )
	: mBrick(brick), mMailbox(mailbox), mScriptControl(scriptControl), mLastRunner(ScriptType::JAVASCRIPT)
{
	mScriptRunnerArray.resize(to_underlying(ScriptType::Size));
	REGISTER_DEVICES_WITH_TEMPLATE(REGISTER_METATYPE)
	if (mailbox) {
			connect(mailbox, &MailboxInterface::newMessage, this, [this](int senderNumber, const QString &message){
				Q_EMIT sendMailboxMessage(QString("mail: sender: %1 contents: %2")
									 .arg(senderNumber)
									 .arg(message)
									 );
			});
	}
	mScriptControl->setParent(this);

	connect(this, &TrikScriptRunnerInterface::startedScript,
			this, [this](const QString &, int) { mScriptRunning = true; });
	connect(this, &TrikScriptRunnerInterface::startedDirectScript,
			this, [this](int) { mScriptRunning = true; });
	connect(this, &TrikScriptRunnerInterface::completed,
			this, [this](const QString &, int) { mScriptRunning = false; });

#ifndef TRIK_NOPYTHON
	// TrikPythonRunner must be initialized early during trikGui startup;
	// otherwise the first script execution can take over 6 seconds.
	fetchRunner(ScriptType::PYTHON);
#endif
}

TrikScriptRunner::TrikScriptRunner(trikControl::BrickInterface &brick
								   , trikNetwork::MailboxInterface * const mailbox
								   )
	: TrikScriptRunner(brick, mailbox, new ScriptExecutionControl(&brick))
{
}

void TrikScriptRunner::setDefaultRunner(ScriptType t)
{
	mLastRunner = t;
}

void TrikScriptRunner::setDefaultRunner(const QString &languageExtension)
{
	if (languageExtension.contains("js")) {
		mLastRunner = ScriptType::JAVASCRIPT;
	}
#ifndef TRIK_NOPYTHON
	else if (languageExtension.contains("py")) {
		mLastRunner = ScriptType::PYTHON;
	}
#endif
}

void TrikScriptRunner::registerUserFunction(const QString &name, QScriptEngine::FunctionSignature function)
{
	fetchRunner(mLastRunner)->registerUserFunction(name, function);
}

void TrikScriptRunner::addCustomEngineInitStep(const std::function<void (QScriptEngine *)> &step)
{
	// The function is only called during JavaScript engine registration
	fetchRunner(ScriptType::JAVASCRIPT)->addCustomEngineInitStep(step);
}

bool TrikScriptRunner::wasError()
{
	return fetchRunner(mLastRunner)->wasError();
}

QStringList TrikScriptRunner::knownMethodNames() const
{
	return mScriptRunnerArray[to_underlying(mLastRunner)]->knownMethodNames();
}

QStringList TrikScriptRunner::knownMethodNamesFor(ScriptType t)
{
	return fetchRunner(t)->knownMethodNames();
}

// Removing the default parameter would require changes in many other places.
// TODO: revisit later.
// NOLINTBEGIN(google-default-arguments)
void TrikScriptRunner::run(const QString &script, const QString &fileName)
{
#ifndef TRIK_NOPYTHON
	if (fileName.endsWith(".py")) {
		run(script, ScriptType::PYTHON, fileName);
	} else
#endif
	{ // default JS
		run(script, ScriptType::JAVASCRIPT, fileName);
	}
}
// NOLINTEND(google-default-arguments)

TrikScriptRunnerInterface * TrikScriptRunner::fetchRunner(ScriptType stype)
{
	auto & cell = mScriptRunnerArray[to_underlying(stype)];
	if (cell == nullptr) { // lazy creation
		switch (stype) {
			case ScriptType::JAVASCRIPT:
				QSharedPointer<TrikScriptRunnerInterface>(
							new TrikJavaScriptRunner(&mBrick, mMailbox, mScriptControl)).swap(cell);
				break;
#ifndef TRIK_NOPYTHON
			case ScriptType::PYTHON:
				QSharedPointer<TrikScriptRunnerInterface>(
							new TrikPythonRunner(&mBrick, mMailbox, mScriptControl)).swap(cell);
				break;
#endif
			case ScriptType::CPP:
				QSharedPointer<TrikScriptRunnerInterface>(
							new TrikCppRunner(&mBrick, mMailbox, mScriptControl)).swap(cell);
				break;
			default:
				QLOG_ERROR() << "Can't handle script with unrecognized type: " << to_underlying(stype);
				return nullptr;
		}
		// subscribe on wrapped objects signals
		connect(&*cell, &TrikScriptRunnerInterface::completed, this, &TrikScriptRunnerInterface::completed);
		connect(&*cell, &TrikScriptRunnerInterface::startedScript, this, &TrikScriptRunnerInterface::startedScript);
		connect(&*cell, &TrikScriptRunnerInterface::startedDirectScript
				, this, &TrikScriptRunnerInterface::startedDirectScript);
		connect(&*cell, &TrikScriptRunnerInterface::textInStdOut, this, &TrikScriptRunnerInterface::textInStdOut);
	}

	setDefaultRunner(stype);

	return cell.data();
}

void TrikScriptRunner::run(const QString &script, ScriptType stype, const QString &fileName)
{
	if (mAbortInProgress) {
		// Abort is in progress (we're inside wait.exec()). Save as pending — will run after abort finishes.
		QLOG_INFO() << "Run deferred, abort in progress";
		mPendingRun.reset(new PendingRun{script, stype, fileName});
		return;
	}

	abort();

	// Abort() calls runPending() at the end — if a newer run arrived during the wait
	// or during resetBrick()'s inner loop, it was already started (mScriptRunning=true).
	// Skip this older run to avoid double-starting.
	if (mScriptRunning) {
		QLOG_INFO() << "Run skipped, pending script was started inside abort";
		return;
	}

	// startedScript arrives via queued connection (worker thread),
	// so the next run() call could arrive before mScriptRunning becomes true from the signal.
	mScriptRunning = true;
	fetchRunner(stype)->run(script, fileName);
}

void TrikScriptRunner::runPending()
{
	if (!mPendingRun) {
		return;
	}
	auto pending = *mPendingRun;
	mPendingRun.reset();
	run(pending.script, pending.stype, pending.fileName);
}

void TrikScriptRunner::runDirectCommand(const QString &command)
{
	fetchRunner(mLastRunner)->runDirectCommand(command);
}

void TrikScriptRunner::abort()
{
	mPendingRun.reset();

	if (!mScriptRunning) {
		return;
	}

	if (mAbortInProgress) {
		return;
	}

	mAbortInProgress = true;
	QEventLoop wait;
	auto conn = connect(this, &TrikScriptRunnerInterface::completed,
						&wait, &QEventLoop::quit);
	fetchRunner(mLastRunner)->abort();
	// Do not proceed with further script launch code — instead wait for complete 
	// termination until the completed signal arrives. Since everything is asynchronous, 
	// exact on-time stopping is not guaranteed, but this works in practice and fixes 
	// the button-spamming hang in TrikStudio and on the controller.
	wait.exec();
	disconnect(conn);

	// Keep mAbortInProgress=true during resetBrick() — it internally spins its own
	// QEventLoop (cross-thread invoke). Any run() calls arriving there will be
	// deferred as pending and started after abort() fully returns.
	fetchRunner(mLastRunner)->resetBrick();
	mAbortInProgress = false;
	// If a run() arrived during wait.exec() or resetBrick()'s inner loop, start it now.
	// This covers the case where abort() is called directly (not via run()), e.g. from abortExecution().
	runPending();
}

void TrikScriptRunner::resetBrick()
{
	fetchRunner(mLastRunner)->resetBrick();
}

void TrikScriptRunner::brickBeep()
{
	fetchRunner(mLastRunner)->brickBeep();
}

void TrikScriptRunner::setWorkingDirectory(const QString &workingDir)
{
	fetchRunner(mLastRunner)->setWorkingDirectory(workingDir);
}

void TrikScriptRunnerInterface::Helper::collectMethodNames(QSet<QString> &result, const QMetaObject *obj)
{
	QSet<const QMetaObject *> visited;
	QStack<const QMetaObject *> stack;
	stack.push(obj);

	while (!stack.isEmpty()) {
		const QMetaObject *cur = stack.pop();
		if (!cur || visited.contains(cur)) {
			continue;
		}
		visited.insert(cur);

		for (int i = cur->methodOffset(); i < cur->methodCount(); ++i) {
			const auto metaMethod = cur->method(i);
			result.insert(QString::fromLatin1(metaMethod.name()));

			const int returnTypeId = metaMethod.returnType();
			if (const QMetaObject *next = QMetaType::metaObjectForType(returnTypeId)) {
				if (!visited.contains(next)) {
					stack.push(next);
				}
			}
		}
	}
}
