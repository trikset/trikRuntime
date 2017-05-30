#include <QsLog.h>

#include "PythonQt.h"
#include "trikPythonRunner.h"
#include "PyTrikControl0.h"

void PythonQt_init_PyTrikControl(PyObject* module);

using namespace trikScriptRunner;

TrikPythonRunner::TrikPythonRunner(trikControl::BrickInterface &brick
								   , trikNetwork::MailboxInterface * const mailbox
								   ) {
	PythonQt::init(PythonQt::IgnoreSiteModule);
	mainContext = PythonQt::self()->getMainModule();
	PythonQt_init_PyTrikControl(mainContext);
	mainContext.addObject("brick", &brick);
}

TrikPythonRunner::~TrikPythonRunner() {}

void TrikPythonRunner::run(const QString &script, const QString &fileName)
{
	mainContext.evalScript(script);
	mainContext.evalScript("raise SystemExit"); // force exit
	emit completed("", 0);
}

void TrikPythonRunner::registerUserFunction(const QString &name, QScriptEngine::FunctionSignature function) {}
void TrikPythonRunner::addCustomEngineInitStep(const std::function<void (QScriptEngine *)> &step) {}

void TrikPythonRunner::runDirectCommand(const QString &command) {}
void TrikPythonRunner::abort() {}
void TrikPythonRunner::brickBeep() {}

void TrikPythonRunner::onScriptStart(int scriptId) {}
void TrikPythonRunner::sendMessageFromMailBox(int senderNumber, const QString &message) {}
