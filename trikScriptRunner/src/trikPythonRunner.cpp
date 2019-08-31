/* Copyright 2018 Iakov Kirilenko, CyberTech Labs Ltd.
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

#include "trikPythonRunner.h"

#include "src/pythonEngineWorker.h"
#include <stdexcept>

using namespace trikScriptRunner;

TrikPythonRunner::TrikPythonRunner(trikControl::BrickInterface &brick
								   , trikNetwork::MailboxInterface * const mailbox
								   )
	: mScriptEngineWorker(new PythonEngineWorker(brick, mailbox))
{
	if (mailbox) {
		connect(mailbox,  &trikNetwork::MailboxInterface::newMessage
				, this, &TrikPythonRunner::sendMessageFromMailBox);
	}

	mScriptEngineWorker->moveToThread(&mWorkerThread);

	connect(mScriptEngineWorker, &PythonEngineWorker::completed
			, this, &TrikPythonRunner::completed);
	connect(mScriptEngineWorker, &PythonEngineWorker::startedScript
			, this, &TrikPythonRunner::onScriptStart);

	mWorkerThread.start();

	QMetaObject::invokeMethod(mScriptEngineWorker, "init"); /// create Python main module

	QLOG_INFO() << "Starting TrikPythonRunner worker thread" << &mWorkerThread;
}

TrikPythonRunner::~TrikPythonRunner()
{
	mScriptEngineWorker->stopScript();
	mScriptEngineWorker->deleteLater();

	QMetaObject::invokeMethod(&mWorkerThread, "quit");
	mWorkerThread.wait(1000);

	mWorkerThread.deleteLater();
}

void TrikPythonRunner::run(const QString &script, const QString &fileName)
{
	Q_UNUSED(fileName);
	mScriptEngineWorker->stopScript();
	mScriptEngineWorker->run(script);
}

void TrikPythonRunner::registerUserFunction(const QString &name, QScriptEngine::FunctionSignature function)
{
	Q_UNUSED(name);
	Q_UNUSED(function);
	throw std::logic_error("Not implemented");
}

void TrikPythonRunner::addCustomEngineInitStep(const std::function<void (QScriptEngine *)> &step)
{
	Q_UNUSED(step);
	throw std::logic_error("Not implemented");
}

void TrikPythonRunner::brickBeep()
{
	QMetaObject::invokeMethod(mScriptEngineWorker, "brickBeep");
}

void TrikPythonRunner::runDirectCommand(const QString &command)
{
	QLOG_INFO() << "TrikPythonRunner: new direct command" << command;
	mScriptEngineWorker->runDirect(command);
}

void TrikPythonRunner::abort()
{
	mScriptEngineWorker->stopScript();
	mScriptEngineWorker->resetBrick();
}

void TrikPythonRunner::onScriptStart(int scriptId)
{
	emit startedDirectScript(scriptId);
}

void TrikPythonRunner::sendMessageFromMailBox(int senderNumber, const QString &message)
{
	emit sendMessage(QString("mail: sender: %1 contents: %2")
					 .arg(senderNumber)
					 .arg(message)
					 );
}

QStringList TrikPythonRunner::knownMethodNames() const
{
	return {};
}
