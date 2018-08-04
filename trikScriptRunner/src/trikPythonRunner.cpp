#include <QsLog.h>

#include "trikPythonRunner.h"

#include "src/pythonEngineWorker.h"

using namespace trikScriptRunner;

TrikPythonRunner::TrikPythonRunner(trikControl::BrickInterface &brick
								   , trikNetwork::MailboxInterface * const mailbox
								   )
	: mScriptEngineWorker(new PythonEngineWorker(brick, mailbox))
{
	if (mailbox) {
		connect(mailbox, SIGNAL(newMessage(int, QString)), this, SLOT(sendMessageFromMailBox(int, QString)));
	}

	mScriptEngineWorker->moveToThread(&mWorkerThread);

	connect(mScriptEngineWorker, SIGNAL(completed(QString, int)), this, SIGNAL(completed(QString, int)));
	connect(mScriptEngineWorker, SIGNAL(startedScript(int)), this, SLOT(onScriptStart(int)));

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
	mScriptEngineWorker->stopScript();
	mScriptEngineWorker->run(script);
}

void TrikPythonRunner::registerUserFunction(const QString &name, QScriptEngine::FunctionSignature function)
{
	throw "Not implemented";
}

void TrikPythonRunner::addCustomEngineInitStep(const std::function<void (QScriptEngine *)> &step)
{
	throw "Not implemented";
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
	return QStringList();
}
