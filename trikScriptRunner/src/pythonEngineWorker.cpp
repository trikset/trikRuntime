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

#include <QProcess>
#include <QsLog.h>

#include <trikNetwork/mailboxInterface.h>
#include <trikKernel/paths.h>

#include "pythonEngineWorker.h"

using namespace trikScriptRunner;

PythonEngineWorker::PythonEngineWorker(trikControl::BrickInterface &brick
		, trikNetwork::MailboxInterface * const mailbox
		)
	: mBrick(brick)
	, mMailbox(mailbox)
	, mState(ready)
{}

PythonEngineWorker::~PythonEngineWorker()
{
	stopScript();
	mMainContext = PythonQtObjectPtr();
	if (mPyInterpreter) {
		Py_EndInterpreter(mPyInterpreter);
		mPyInterpreter = nullptr;
	}
	if (Py_IsInitialized()) {
		Py_Finalize();
	}
	if (PythonQt::self()) {
		PythonQt::cleanup();
	}
}

void PythonEngineWorker::init()
{
	if (!Py_IsInitialized()) {
		static auto nonConst = wcsdup(L"SOME_PATH"); // leak?
		Py_SetPythonHome(nonConst); //??? Need to set correct one

		auto path = QProcessEnvironment::systemEnvironment().value("TRIK_PYTHONPATH", "");
		// TODO: Now use PYTHONPATH environment variable (default) until fixed
		// Must point to local .zip file
		Py_SetPath(path.toStdWString().data());


/* uncomment for verbosity
		Py_VerboseFlag = 3;
		Py_InspectFlag = 1;
		Py_DebugFlag = 2;
// */
		Py_IsolatedFlag = 1;
		Py_BytesWarningFlag = 3;
		Py_DontWriteBytecodeFlag = 1;
		Py_NoSiteFlag = 1;
		Py_NoUserSiteDirectory = 1;

		Py_Initialize();
	}

	if (!mPyInterpreter) {
	//	mPyInterpreter = Py_NewInterpreter();
	}

	if (!PythonQt::self()) {
		PythonQt::init(PythonQt::RedirectStdOut | PythonQt::PythonAlreadyInitialized);
		PythonQt_QtAll::init();
		connect(PythonQt::self(), &PythonQt::pythonStdErr
			, this, &PythonEngineWorker::updateErrorMessage);
	}

	mMainContext = PythonQt::self()->getMainModule();

	initTrik();
}

void PythonEngineWorker::recreateContext()
{
	mMainContext.evalScript("script.kill()");

	initTrik();
}

void PythonEngineWorker::evalSystemPy()
{
	const QString systemPyPath = trikKernel::Paths::systemScriptsPath() + "system.py";

	if (QFile::exists(systemPyPath)) {
		mMainContext.evalFile(systemPyPath);
	} else {
		QLOG_ERROR() << "system.py not found, path:" << systemPyPath;
	}
}

void PythonEngineWorker::initTrik()
{
	PythonQt_init_PyTrikControl(mMainContext);
	mMainContext.addObject("brick", &mBrick);

	evalSystemPy();
}

void PythonEngineWorker::resetBrick()
{
	QLOG_INFO() << "Stopping robot";

	if (mMailbox) {
		mMailbox->stopWaiting();
		mMailbox->clearQueue();
	}

	mBrick.reset();
}

void PythonEngineWorker::brickBeep()
{
	mBrick.playSound(trikKernel::Paths::mediaPath() + "media/beep_soft.wav");
}

void PythonEngineWorker::stopScript()
{
	QMutexLocker locker(&mScriptStateMutex);

	if (mState == stopping) {
		// Already stopping, so we can do nothing.
		return;
	}

	if (mState == ready) {
		// Engine is ready for execution.
		return;
	}

	QLOG_INFO() << "PythonEngineWorker: stopping script";

	mState = stopping;

	if (mMailbox) {
		mMailbox->stopWaiting();
	}

	QMetaObject::invokeMethod(this, "recreateContext"); /// recreates python module, which we use

	mState = ready;

	/// @todo: is it actually stopped?

	QLOG_INFO() << "PythonEngineWorker: stopping complete";
}

void PythonEngineWorker::run(const QString &script)
{
	QMutexLocker locker(&mScriptStateMutex);
	mState = starting;
	emit startedScript(0);
	QMetaObject::invokeMethod(this, "doRun", Q_ARG(const QString &, script));
}

void PythonEngineWorker::doRun(const QString &script)
{
	mErrorMessage.clear();
	/// When starting script execution (by any means), clear button states.
	mBrick.keys()->reset();

	mMainContext.evalScript(script);

	mState = running;
	QLOG_INFO() << "PythonEngineWorker: evaluation ended";

	if (PythonQt::self()->hadError()) {
		emit completed(mErrorMessage, 0);
	} else {
		emit completed("", 0);
	}
}

void PythonEngineWorker::runDirect(const QString &command)
{
	QMutexLocker locker(&mScriptStateMutex);
	QMetaObject::invokeMethod(this, "doRunDirect", Q_ARG(const QString &, command));
}

void PythonEngineWorker::doRunDirect(const QString &command)
{
	if (PythonQt::self()->hadError()) {
		PythonQt::self()->clearError();
		mErrorMessage.clear();
		recreateContext();
	}
	mMainContext.evalScript(command);
}

void PythonEngineWorker::updateErrorMessage(const QString &err)
{
	mErrorMessage += err;
}

void PythonEngineWorker::onScriptRequestingToQuit()
{
	throw std::logic_error("Not implemented");
}
