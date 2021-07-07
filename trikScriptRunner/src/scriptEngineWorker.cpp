/* Copyright 2013 - 2016 Yurii Litvinov, CyberTech Labs Ltd.
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

#include "scriptEngineWorker.h"

#include <QtCore/QFile>
#include <QtCore/QVector>
#include <QtCore/QTextStream>
#include <QtCore/QMetaMethod>
#include <QtCore/QStringBuilder>
#include <trikKernel/fileUtils.h>
#include <trikKernel/paths.h>
#include "trikScriptRunnerInterface.h"
#include "scriptable.h"
#include "utils.h"

#include <QFileInfo>
#include <QsLog.h>

#define REGISTER_METATYPE_FOR_ENGINE(TYPE) \
	Scriptable<TYPE>::registerMetatype(engine);


using namespace trikScriptRunner;

constexpr auto scriptEngineWorkerName = "__scriptEngineWorker";

QScriptValue include(QScriptContext *context, QScriptEngine *engine)
{
	const auto &filename = context->argument(0).toString();

	const auto & scriptValue = engine->globalObject().property(scriptEngineWorkerName);
	if (auto scriptWorkerValue = qobject_cast<ScriptEngineWorker *> (scriptValue.toQObject())) {
		auto connection = (QThread::currentThread() != engine->thread()) ?
					Qt::BlockingQueuedConnection : Qt::DirectConnection;
		QMetaObject::invokeMethod(scriptWorkerValue, [scriptWorkerValue, filename, engine]()
					{scriptWorkerValue->evalInclude(filename, engine);}, connection);
	}

	return QScriptValue();
}

QScriptValue print(QScriptContext *context, QScriptEngine *engine)
{
	QString result;
	result.reserve(100000);
	int argumentCount = context->argumentCount();
	for (int i = 0; i < argumentCount; ++i) {
		std::function<QString(const QVariant &)> prettyPrinter
			= [&prettyPrinter](QVariant const & elem) {
			auto const &arrayPrettyPrinter = [&prettyPrinter](const QVariantList &array) {
				qint32 arrayLength = array.length();

				if (arrayLength == 0) {
					return QString("[]");
				}

				QString res;
				res.reserve(100000);
				res.append("[" % prettyPrinter(array.first()));

				for(auto i = 1; i < arrayLength; ++i) {
					res.append(", " % prettyPrinter(array.at(i)));
				}

				res.append("]");
				return res;
			};

			return elem.canConvert(QMetaType::QVariantList)
				? arrayPrettyPrinter(elem.toList())
				: elem.toString();
		};
		QScriptValue argument = context->argument(i);
		result.append(prettyPrinter(argument.toVariant()));
	}

	auto scriptValue = engine->globalObject().property("script");

	if (auto script = qobject_cast<TrikScriptControlInterface*> (scriptValue.toQObject())) {
		result.append('\n');
		QTimer::singleShot(0, script, [script, result](){ Q_EMIT script->textInStdOut(result);});
		/// In case of user loop with `print' this gives some time for events to be processed
		script->wait(0);
	}

	return engine->toScriptValue(result);
}

ScriptEngineWorker::ScriptEngineWorker(trikControl::BrickInterface *brick
		, trikNetwork::MailboxInterface * mailbox
		, TrikScriptControlInterface *scriptControl
		)
	: mBrick(brick)
	, mMailbox(mailbox)
	, mScriptControl(scriptControl)
	, mThreading(this, *scriptControl)
	, mWorkingDirectory(trikKernel::Paths::userScriptsPath())
{
	connect(mScriptControl, &TrikScriptControlInterface::quitSignal,
		this, &ScriptEngineWorker::onScriptRequestingToQuit);
	connect(this, &ScriptEngineWorker::getVariables, &mThreading, &Threading::getVariables);
	connect(&mThreading, &Threading::variablesReady, this, &ScriptEngineWorker::variablesReady);

	registerUserFunction("print", print);
	registerUserFunction("include", include);
}

void ScriptEngineWorker::brickBeep()
{
	mBrick->playTone(2500, 20);
}

void ScriptEngineWorker::evalInclude(const QString &filename, QScriptEngine * const engine)
{
	QFileInfo fi(mWorkingDirectory, filename);
	evalExternalFile(fi.absoluteFilePath(), engine);
}

void ScriptEngineWorker::setWorkingDir(const QString &workingDir)
{
	mWorkingDirectory.setPath(workingDir);
}

void ScriptEngineWorker::stopScript()
{
	QMutexLocker locker(&mScriptStateMutex);

	while (mState == starting) {
		// Some script is starting right now, so we are in inconsistent state. Let it start, then stop it.
		locker.unlock();
		QThread::yieldCurrentThread();
		locker.relock();
	}

	if (mState == stopping) {
		// Already stopping, so we can do nothing.
		return;
	}

	if (mState == ready) {
		// Engine is ready for execution.
		return;
	}


	QLOG_INFO() << "ScriptEngineWorker: stopping script";

	mState = stopping;

	mScriptControl->reset();

	if (mMailbox) {
		mMailbox->stopWaiting();
		/// @todo: here script will continue to execute and may execute some statements before it will eventually
		/// be stopped by mThreading.reset(). But if we do mThreading.reset() before mMailbox->stopWaiting(),
		/// we will get deadlock, since mMailbox->stopWaiting() shall be executed in already stopped thread.
		/// Actually we shall stop script engines here, do mMailbox->stopWaiting(), then stop threads.
	}

	QMetaObject::invokeMethod(&mThreading, &Threading::reset, Qt::QueuedConnection);

	if (mDirectScriptsEngine) {
		mDirectScriptsEngine->abortEvaluation();
		QLOG_INFO() << "ScriptEngineWorker : ending interpretation";
		const auto &msg = mDirectScriptsEngine->hasUncaughtException()
				   ? mDirectScriptsEngine->uncaughtException().toString()
				   : "";
		// This method is called from script.quit()
		// Thus deletion of the mDirectScriptsEngine should be postponed
		// Instead of deleteLater() we use zero timer
		QTimer::singleShot(0, this, [this]() { mDirectScriptsEngine.reset(); });

		emit completed(msg, mScriptId);
	}

	mState = ready;

	/// @todo: is it actually stopped?

	QLOG_INFO() << "ScriptEngineWorker: stopping complete";
}

void ScriptEngineWorker::resetBrick()
{
	QLOG_INFO() << "Stopping robot";

	if (mMailbox) {
		mMailbox->stopWaiting();
		mMailbox->clearQueue();
	}

	mBrick->reset();
}

void ScriptEngineWorker::run(const QString &script, int scriptId)
{
	QMutexLocker locker(&mScriptStateMutex);
	startScriptEvaluation(scriptId);
	QMetaObject::invokeMethod(this, std::bind(&ScriptEngineWorker::doRun, this, script));
}

void ScriptEngineWorker::doRun(const QString &script)
{
	/// When starting script execution (by any means), clear button states.
	mBrick->keys()->reset();
	mThreading.startMainThread(script);
	mState = running;
	mThreading.waitForAll();
	const QString error = mThreading.errorMessage();
	QLOG_INFO() << "ScriptEngineWorker: evaluation ended with message" << error;
	emit completed(error, mScriptId);
}

void ScriptEngineWorker::runDirect(const QString &command, int scriptId)
{
	QMutexLocker locker(&mScriptStateMutex);
	if (!mScriptControl->isInEventDrivenMode()) {
		QLOG_INFO() << "ScriptEngineWorker: starting interpretation";
		locker.unlock();
		stopScript();
	}

	QMetaObject::invokeMethod(this, std::bind(&ScriptEngineWorker::doRunDirect, this, command, scriptId));
}

void ScriptEngineWorker::doRunDirect(const QString &command, int scriptId)
{
	if (!mScriptControl->isInEventDrivenMode() && !mDirectScriptsEngine) {
		startScriptEvaluation(scriptId);
		mDirectScriptsEngine.reset(createScriptEngine(false));
		mScriptControl->run();
		mState = running;
	}

	if (mDirectScriptsEngine) {
		mDirectScriptsEngine->evaluate(command);

		/// If script was stopped by quit(), engine will already be reset to nullptr in ScriptEngineWorker::stopScript.
		QString msg;
		if (mDirectScriptsEngine && mDirectScriptsEngine->hasUncaughtException()) {
			QLOG_INFO() << "ScriptEngineWorker : ending interpretation of direct script";
			msg = mDirectScriptsEngine->uncaughtException().toString();
			mDirectScriptsEngine.reset();
		}
		Q_EMIT completed(msg, mScriptId);
	}
}

void ScriptEngineWorker::startScriptEvaluation(int scriptId)
{
	QLOG_INFO() << "ScriptEngineWorker: starting script" << scriptId << ", thread:" << QThread::currentThread();
	mState = starting;
	mScriptId = scriptId;
	emit startedScript(mScriptId);
}

void ScriptEngineWorker::evalExternalFile(const QString & filepath, QScriptEngine * const engine)
{
	if (QFileInfo::exists(filepath)) {
		engine->evaluate(trikKernel::FileUtils::readFromFile(filepath), filepath);
		if (engine->hasUncaughtException()) {
			const auto line = engine->uncaughtExceptionLineNumber();
			const auto & message = engine->uncaughtException().toString();
			const auto & backtrace = engine->uncaughtExceptionBacktrace().join("\n");
			const auto & error = tr("Line %1: %2").arg(QString::number(line), message) + "\nBacktrace"+ backtrace;
			emit completed(error, mScriptId);
			QLOG_ERROR() << "Uncaught exception with error" << error;
		}
	} else {
		emit completed(tr("File %1 not found").arg(filepath), mScriptId);
		QLOG_ERROR() << "File for eval not found, path:" << filepath;
	}
}

void ScriptEngineWorker::onScriptRequestingToQuit()
{
	if (!mScriptControl->isInEventDrivenMode()) {
		// Somebody erroneously called script.quit() before entering event loop, so we must force event loop for script
		// and only then quit, to send completed() signal properly.
		mScriptControl->run();
	}

	stopScript();
}

static QScriptValue timeValToScriptValue(QScriptEngine *engine, const trikKernel::TimeVal &in)
{
	QScriptValue obj = engine->newObject();
	obj.setProperty("mcsec", in.packedUInt32());
	return obj;
}

static void timeValFromScriptValue(const QScriptValue &object, trikKernel::TimeVal &out)
{
	out = trikKernel::TimeVal(0, object.property("mcsec").toInt32());
}

QScriptEngine * ScriptEngineWorker::createScriptEngine(bool supportThreads)
{
	QScriptEngine *engine = new QScriptEngine();
	QLOG_INFO() << "New script engine" << engine << ", thread:" << QThread::currentThread();

	REGISTER_DEVICES_WITH_TEMPLATE(REGISTER_METATYPE_FOR_ENGINE)
	REGISTER_METATYPE_FOR_ENGINE(trikScriptRunner::Threading)

	Scriptable<QTimer>::registerMetatype(engine);
	qScriptRegisterMetaType(engine, &timeValToScriptValue, &timeValFromScriptValue);
	qScriptRegisterSequenceMetaType<QVector<int32_t>>(engine);
	qScriptRegisterSequenceMetaType<QStringList>(engine);
	qScriptRegisterSequenceMetaType<QVector<uint8_t>>(engine);

	engine->globalObject().setProperty("brick", engine->newQObject(mBrick));
	engine->globalObject().setProperty("script", engine->newQObject(mScriptControl));
	engine->globalObject().setProperty(scriptEngineWorkerName, engine->newQObject(this));

	if (mMailbox) {
		engine->globalObject().setProperty("mailbox", engine->newQObject(mMailbox));
	}

	// Gamepad can still be accessed from script as brick.gamepad(), 'gamepad' variable is here for backwards
	// compatibility.
	if (auto gamepad = mBrick->gamepad()) {
		engine->globalObject().setProperty("gamepad", engine->newQObject(gamepad));
	}

	if (supportThreads) {
		engine->globalObject().setProperty("Threading", engine->newQObject(&mThreading));
	}

	evalSystemJs(engine);

	for (const auto &step : mCustomInitSteps) {
		step(engine);
	}

	engine->setProcessEventsInterval(1);
	return engine;
}

QScriptEngine *ScriptEngineWorker::copyScriptEngine(const QScriptEngine * const original)
{
	QScriptEngine *const result = createScriptEngine();

	QScriptValue globalObject = result->globalObject();
	Utils::copyRecursivelyTo(original->globalObject(), globalObject, result);
	result->setGlobalObject(globalObject);

	// We need to re-eval system.js after global object copying because functions did not get copied by
	// copyRecursivelyTo, and existing ones were overwritten by copying.
	evalSystemJs(result);

	return result;
}

void ScriptEngineWorker::registerUserFunction(const QString &name, QScriptEngine::FunctionSignature function)
{
	mRegisteredUserFunctions[name] = function;
}

void ScriptEngineWorker::addCustomEngineInitStep(const std::function<void (QScriptEngine *)> &step)
{
	mCustomInitSteps.append(step);
}

void ScriptEngineWorker::evalSystemJs(QScriptEngine * const engine)
{
	const QString systemJsPath = trikKernel::Paths::systemScriptsPath() + "system.js";
	evalExternalFile(systemJsPath, engine);

	for (const auto &functionName : mRegisteredUserFunctions.keys()) {
		QScriptValue functionValue = engine->newFunction(mRegisteredUserFunctions[functionName]);
		engine->globalObject().setProperty(functionName, functionValue);
	}
}

QStringList ScriptEngineWorker::knownMethodNames() const
{
	QSet<QString> result = {"brick", "script", "threading"};
	TrikScriptRunnerInterface::Helper::collectMethodNames(result, &trikControl::BrickInterface::staticMetaObject);
	TrikScriptRunnerInterface::Helper::collectMethodNames(result, mScriptControl->metaObject());
	if (mMailbox) {
		result.insert("mailbox");
		TrikScriptRunnerInterface::Helper::collectMethodNames(result, mMailbox->metaObject());
	}
	TrikScriptRunnerInterface::Helper::collectMethodNames(result, mThreading.metaObject());
	return result.values();
}
