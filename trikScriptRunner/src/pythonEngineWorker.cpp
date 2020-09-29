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
#include <QVector>

#include <trikNetwork/mailboxInterface.h>
#include <trikKernel/paths.h>
#include <trikKernel/exceptions/internalErrorException.h>
#include <trikScriptRunnerInterface.h>

#include "pythonEngineWorker.h"
#include <Python.h>
#include "PythonQtConversion.h"

using namespace trikScriptRunner;

QAtomicInt PythonEngineWorker::initCounter = 0;

static int quitFromPython(void*) {
	PyErr_SetInterrupt();
	return 0;
}

static void abortPythonInterpreter() {
	if(!Py_IsInitialized()) {
		return;
	}
	PythonQtGILScope _;
	Py_AddPendingCall(&quitFromPython, nullptr);
}

PythonEngineWorker::PythonEngineWorker(trikControl::BrickInterface &brick
		, trikNetwork::MailboxInterface * const mailbox
		)
	: mBrick(brick)
	, mScriptExecutionControl(new ScriptExecutionControl(brick))
	, mMailbox(mailbox)
	, mWorkingDirectory(trikKernel::Paths::userScriptsPath())
{}

PythonEngineWorker::~PythonEngineWorker()
{
	stopScript();
	{
		// In python at least before 3.7 (3.5,3.6)
		// we need to make all pending calls before the context
		// is destroyed, otherwise python crashes
		PythonQtGILScope _;
		Py_MakePendingCalls();
		mMainContext = nullptr;
		if (mPyInterpreter) {
			Py_EndInterpreter(mPyInterpreter);
			mPyInterpreter = nullptr;
		}
	}

	if (--initCounter == 0) {
#if PY_MAJOR_VERSION != 3
#error "Unsupported PYTHON version"
#endif
#if PY_MINOR_VERSION < 6
		Py_Finalize();
#else
		if (Py_FinalizeEx()) {
			QLOG_ERROR() << "Failed to finalize python engine";
		}

#endif

		if (PythonQt::self()) {
			PythonQt::cleanup();
		}

		PyMem_RawFree(mProgramName);
		PyMem_RawFree(mPythonPath);
	}
}

void PythonEngineWorker::init()
{
	if (initCounter++ == 0) {

		QLOG_INFO() << "Built with python:" << PY_VERSION << QString::number(PY_VERSION_HEX, 16);
		QLOG_INFO() << "Running with python:" << Py_GetVersion();
		if (strncmp(PY_VERSION, Py_GetVersion(), 4)) {
			auto const &e = QString("Incompatible Python runtime detected. Expecting version %1, but found %2")
							.arg(PY_VERSION).arg(Py_GetVersion());
			throw trikKernel::InternalErrorException(e);
		}
		constexpr auto varName = "TRIK_PYTHONPATH";
		auto const &path = QProcessEnvironment::systemEnvironment().value(varName);
		if (path.isEmpty()) {
			auto const &e = QString("%1 must be set to correct value").arg(varName);
			QLOG_FATAL() << e;
			throw trikKernel::InternalErrorException(e);
		} else {
			QLOG_INFO() << varName << ":" << path;
		}

		/// TODO: Must point to local .zip file
		mPythonPath = Py_DecodeLocale(path.toStdString().data(), nullptr);
		Py_SetPath(mPythonPath);

		mProgramName = Py_DecodeLocale("trikPythonRuntime", nullptr);
		Py_SetProgramName(mProgramName);

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
#if PY_MAJOR_VERSION != 3
#error "Unsupported PYTHON version"
#else
#if PY_MINOR_VERSION < 6
		Py_Initialize();
#else
		Py_InitializeEx(true); // Initialize handlers, because we need PyErr_SetInterrupt()
#endif

#if PY_MINOR_VERSION < 7
		PyEval_InitThreads();
#endif
#endif

		constexpr auto extractVersionCommand = "(sys.version_info.major,sys.version_info.minor)";
		PythonQtObjectPtr dict;
		dict.setNewRef(PyDict_New());
		PyMapping_SetItemString(dict.object(), "sys", PyImport_ImportModule("sys"));
		PythonQtObjectPtr ver;
		ver.setNewRef(PyRun_StringFlags(extractVersionCommand, Py_eval_input, dict.object(), dict.object(), nullptr));
		if (ver) {
			auto major = PyLong_AsLong(PyTuple_GetItem(ver, 0));
			auto minor = PyLong_AsLong(PyTuple_GetItem(ver, 1));
			if (major != PY_MAJOR_VERSION || minor != PY_MINOR_VERSION) {
				auto const &e = QString("Incompatible Python library detected. Expecting version %1, but found %2.%3")
								.arg(PY_VERSION).arg(major).arg(minor);
				throw trikKernel::InternalErrorException(e);
			}
		} else {
			auto e = QString("Failed to extract Python version from provided library, check %1").arg(varName);
			QLOG_FATAL() << e;
			throw trikKernel::InternalErrorException(e);
		}
	}

	if (!mPyInterpreter) {
//		mPyInterpreter = Py_NewInterpreter();
	}

	if (!PythonQt::self()) {
		PythonQt::setEnableThreadSupport(true);
		PythonQtGILScope _;
		PythonQt::init(PythonQt::RedirectStdOut | PythonQt::PythonAlreadyInitialized);
		connect(PythonQt::self(), &PythonQt::pythonStdErr, this, &PythonEngineWorker::updateErrorMessage);
		connect(PythonQt::self(), &PythonQt::pythonStdOut, this, [this](const QString& str){
			QTimer::singleShot(0, this, [this, str](){ Q_EMIT this->textInStdOut(str);});
			mScriptExecutionControl->wait(0);
		});
		PythonQtRegisterListTemplateConverter(QVector, uint8_t)
		PythonQt_QtAll::init();
	}
	if (!mMainContext) {
		mMainContext = PythonQt::self()->getMainModule();
		recreateContext();
		if (!initTrik()) {
			QLOG_FATAL() << "Failed to init TRIK runtime for Python";
		}
	}
	emit inited();
}

bool PythonEngineWorker::recreateContext()
{
	{
		PythonQtGILScope _;
		Py_MakePendingCalls();
		PyErr_CheckSignals();
		PyErr_Clear();
	}
	PythonQt::self()->clearError();
	return true;
}

bool PythonEngineWorker::evalSystemPy()
{
	const QString systemPyPath = trikKernel::Paths::systemScriptsPath() + "system.py";

	if (!QFileInfo::exists(systemPyPath)) {
		QLOG_ERROR() << "system.py not found, path:" << systemPyPath;
		return false;
	}

	// HACK: to avoid duplicate system.py try to check if basic feature like script.wait works.
	mMainContext.evalScript("script.wait(0)");
	if (PythonQt::self()->hadError()) {
		// HACK: no script.wait means usually a problem with system.py, let's try to include it
		PythonQt::self()->clearError();
		mMainContext.evalFile(systemPyPath);
		if (PythonQt::self()->hadError()) {
			QLOG_ERROR() << "Failed to eval system.py";
			return false;
		}
	}

	return true;
}

void PythonEngineWorker::addSearchModuleDirectory(const QDir &path)
{
	if (path.path().indexOf("'") != -1)
		return;

	mMainContext.evalScript("import sys; (lambda x: sys.path.append(x) if not x in sys.path else None)('"
							+ path.path() + "')");
}

bool PythonEngineWorker::initTrik()
{
	PythonQt_init_PyTrikControl(mMainContext);
	mMainContext.addObject("brick", &mBrick);
	mMainContext.addObject("script_cpp", mScriptExecutionControl.data());
	mMainContext.addObject("mailbox", mMailbox);

	return evalSystemPy();
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

	if (QThread::currentThread() != thread()) {
		abortPythonInterpreter();
	} else {
		QLOG_FATAL() << "Attempt to abort Python from main thread.";
	}

	if (mMailbox) {
		mMailbox->stopWaiting();
	}

	mScriptExecutionControl->reset();

	mState = ready;

	/// @todo: is it actually stopped?

	QLOG_INFO() << "PythonEngineWorker: stopping complete";
}

QStringList PythonEngineWorker::knownNames() const
{
	QSet<QString> result = {"brick", "script", "threading"};
	TrikScriptRunnerInterface::Helper::collectMethodNames(result, &trikControl::BrickInterface::staticMetaObject);
/// TODO:	TrikScriptRunnerInterface::Helper::collectMethodNames(result, mScriptControl.metaObject());
	if (mMailbox) {
		result.insert("mailbox");
		TrikScriptRunnerInterface::Helper::collectMethodNames(result, mMailbox->metaObject());
	}
/// TODO:	TrikScriptRunnerInterface::Helper::collectMethodNames(result, mThreading.metaObject());
	return result.toList();
}

void PythonEngineWorker::setWorkingDirectory(const QDir &workingDir)
{
	mWorkingDirectory = workingDir;
}

void PythonEngineWorker::run(const QString &script, const QFileInfo &scriptFile)
{
	QMutexLocker locker(&mScriptStateMutex);
	mState = starting;
	QMetaObject::invokeMethod(this, [this, script, scriptFile](){this->doRun(script, scriptFile);});
}

void PythonEngineWorker::doRun(const QString &script, const QFileInfo &scriptFile)
{
	emit startedScript("", 0);
	mErrorMessage.clear();
	/// When starting script execution (by any means), clear button states.
	mBrick.keys()->reset();
	mState = running;
	auto ok = recreateContext();
	if (!ok) {
		emit completed(mErrorMessage,0);
		return;
	}

	addSearchModuleDirectory(mWorkingDirectory.canonicalPath());
	if (scriptFile.isFile()) {
		addSearchModuleDirectory(scriptFile.canonicalPath());
	}

	mMainContext.evalScript(script);

	QLOG_INFO() << "PythonEngineWorker: evaluation ended";

	auto wasError = mState != ready && PythonQt::self()->hadError();
	mState = ready;
	if (wasError) {
		emit completed(mErrorMessage, 0);
	} else {
		emit completed("", 0);
	}
}

void PythonEngineWorker::runDirect(const QString &command)
{
	QMutexLocker locker(&mScriptStateMutex);
	QMetaObject::invokeMethod(this, "doRunDirect", Q_ARG(QString, command));
}

void PythonEngineWorker::doRunDirect(const QString &command)
{
	emit startedDirectScript(0);
	if (PythonQt::self()->hadError()) {
		PythonQt::self()->clearError();
		mErrorMessage.clear();
		recreateContext();
	}
	mMainContext.evalScript(command);
	emit completed(mErrorMessage, 0);
}

void PythonEngineWorker::updateErrorMessage(const QString &err)
{
	mErrorMessage += err;
}

void PythonEngineWorker::onScriptRequestingToQuit()
{
	throw std::logic_error("Not implemented");
}
