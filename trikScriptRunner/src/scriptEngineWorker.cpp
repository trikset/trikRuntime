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
#include <QtCore/QSignalMapper>

#include <trikKernel/fileUtils.h>
#include <trikKernel/paths.h>
#include <trikKernel/timeVal.h>
#include <trikControl/batteryInterface.h>
#include <trikControl/colorSensorInterface.h>
#include <trikControl/displayInterface.h>
#include <trikControl/encoderInterface.h>
#include <trikControl/eventCodeInterface.h>
#include <trikControl/eventDeviceInterface.h>
#include <trikControl/eventInterface.h>
#include <trikControl/gamepadInterface.h>
#include <trikControl/gyroSensorInterface.h>
#include <trikControl/lineSensorInterface.h>
#include <trikControl/motorInterface.h>
#include <trikControl/objectSensorInterface.h>
#include <trikControl/soundSensorInterface.h>
#include <trikControl/sensorInterface.h>
#include <trikControl/vectorSensorInterface.h>
#include <trikNetwork/mailboxInterface.h>

#include "scriptable.h"
#include "utils.h"

#include <QsLog.h>

using namespace trikScriptRunner;
using namespace trikControl;
using namespace trikNetwork;

Q_DECLARE_METATYPE(BatteryInterface*)
Q_DECLARE_METATYPE(ColorSensorInterface*)
Q_DECLARE_METATYPE(FifoInterface*)
Q_DECLARE_METATYPE(DisplayInterface*)
Q_DECLARE_METATYPE(EncoderInterface*)
Q_DECLARE_METATYPE(EventCodeInterface*)
Q_DECLARE_METATYPE(EventDeviceInterface*)
Q_DECLARE_METATYPE(EventInterface*)
Q_DECLARE_METATYPE(GamepadInterface*)
Q_DECLARE_METATYPE(GyroSensorInterface*)
Q_DECLARE_METATYPE(KeysInterface*)
Q_DECLARE_METATYPE(LedInterface*)
Q_DECLARE_METATYPE(LineSensorInterface*)
Q_DECLARE_METATYPE(MailboxInterface*)
Q_DECLARE_METATYPE(MotorInterface*)
Q_DECLARE_METATYPE(ObjectSensorInterface*)
Q_DECLARE_METATYPE(SoundSensorInterface*)
Q_DECLARE_METATYPE(SensorInterface*)
Q_DECLARE_METATYPE(Threading*)
Q_DECLARE_METATYPE(VectorSensorInterface*)
Q_DECLARE_METATYPE(QVector<int>)
Q_DECLARE_METATYPE(trikKernel::TimeVal)
Q_DECLARE_METATYPE(QTimer*)

QScriptValue print(QScriptContext *context, QScriptEngine *engine)
{
	QString result;
	for (int i = 0; i < context->argumentCount(); ++i) {
		if (i > 0) {
			result.append(" ");
		}

		result.append(context->argument(i).toString());
	}

	QTextStream(stdout) << result << "\n";
	auto scriptValue = engine->globalObject().property("script");
	auto script = dynamic_cast<ScriptExecutionControl*> (scriptValue.toQObject());
	if (script) {
		QMetaObject::invokeMethod(script, "sendMessage", Q_ARG(QString, QString("print: %1").arg(result)));
//		If this does not work then it can be workarrounded:
//		QSignalMapper *mapper = new QSignalMapper();
//		QObject::connect(mapper, SIGNAL(mapped(QString)), script, SIGNAL(sendMessage(QString)), Qt::DirectConnection);
//		mapper->setMapping(nullptr, QString("print: %1").arg(result));
//		mapper->map(nullptr);
//		delete mapper;
	}

	return engine->toScriptValue(result);
}

QScriptValue timeInterval(QScriptContext *context, QScriptEngine *engine)
{
	int result = trikKernel::TimeVal::timeInterval(context->argument(0).toInteger(), context->argument(1).toInteger());
	return engine->toScriptValue(result);
}

ScriptEngineWorker::ScriptEngineWorker(trikControl::BrickInterface &brick
		, trikNetwork::MailboxInterface * const mailbox
		, ScriptExecutionControl &scriptControl
		)
	: mBrick(brick)
	, mMailbox(mailbox)
	, mScriptControl(scriptControl)
	, mThreading(this, scriptControl)
	, mDirectScriptsEngine(nullptr)
	, mScriptId(0)
	, mState(ready)
{
	connect(&mScriptControl, SIGNAL(quitSignal()), this, SLOT(onScriptRequestingToQuit()));

	registerUserFunction("print", print);
	registerUserFunction("timeInterval", timeInterval);
}

void ScriptEngineWorker::brickBeep()
{
	mBrick.playSound(trikKernel::Paths::mediaPath() + "media/beep_soft.wav");
}

void ScriptEngineWorker::stopScript()
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

	while (mState == starting) {
		// Some script is starting right now, so we are in inconsistent state. Let it start, then stop it.
		QThread::yieldCurrentThread();
	}

	QLOG_INFO() << "ScriptEngineWorker: stopping script";

	mState = stopping;

	mScriptControl.reset();

	if (mMailbox) {
		mMailbox->stopWaiting();
		/// @todo: here script will continue to execute and may execute some statements before it will eventually
		/// be stopped by mThreading.reset(). But if we do mThreading.reset() before mMailbox->stopWaiting(),
		/// we will get deadlock, since mMailbox->stopWaiting() shall be executed in already stopped thread.
		/// Actually we shall stop script engines here, do mMailbox->stopWaiting(), then stop threads.
	}

	mThreading.reset();

	if (mDirectScriptsEngine) {
		mDirectScriptsEngine->abortEvaluation();
		QLOG_INFO() << "ScriptEngineWorker : ending interpretation";
		emit completed(mDirectScriptsEngine->hasUncaughtException()
						? mDirectScriptsEngine->uncaughtException().toString()
						: ""
				, mScriptId);

		mDirectScriptsEngine->deleteLater();
		mDirectScriptsEngine = nullptr;
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

	mBrick.reset();
}

void ScriptEngineWorker::run(const QString &script, int scriptId)
{
	QMutexLocker locker(&mScriptStateMutex);
	startScriptEvaluation(scriptId);
	QMetaObject::invokeMethod(this, "doRun", Q_ARG(const QString &, script));
}

void ScriptEngineWorker::doRun(const QString &script)
{
	/// When starting script execution (by any means), clear button states.
	mBrick.keys()->reset();

	mThreading.startMainThread(script);
	mState = running;
	mThreading.waitForAll();
	const QString error = mThreading.errorMessage();
	QLOG_INFO() << "ScriptEngineWorker: evaluation ended with message" << error;
	emit completed(error, mScriptId);
}

void ScriptEngineWorker::runDirect(const QString &command, int scriptId)
{
	qDebug() << "ScriptEngineWorker::runDirect";

	QMutexLocker locker(&mScriptStateMutex);
	if (!mScriptControl.isInEventDrivenMode()) {
		QLOG_INFO() << "ScriptEngineWorker: starting interpretation";
		locker.unlock();
		stopScript();
	}

	QMetaObject::invokeMethod(this, "doRunDirect", Q_ARG(const QString &, command), Q_ARG(int, scriptId));
}

void ScriptEngineWorker::doRunDirect(const QString &command, int scriptId)
{
	if (!mScriptControl.isInEventDrivenMode() && !mDirectScriptsEngine) {
		startScriptEvaluation(scriptId);
		mDirectScriptsEngine = createScriptEngine(false);
		mScriptControl.run();
		mState = running;
	}

	if (mDirectScriptsEngine) {
		mDirectScriptsEngine->evaluate(command);

		/// If script was stopped by quit(), engine will already be reset to nullptr in ScriptEngineWorker::stopScript.
		if (mDirectScriptsEngine && mDirectScriptsEngine->hasUncaughtException()) {
			QLOG_INFO() << "ScriptEngineWorker : ending interpretation of direct script";
			emit completed(mDirectScriptsEngine->hasUncaughtException()
					? mDirectScriptsEngine->uncaughtException().toString()
					: "", mScriptId);
			mDirectScriptsEngine->deleteLater();
			mDirectScriptsEngine = nullptr;
		}
	}
}

void ScriptEngineWorker::startScriptEvaluation(int scriptId)
{
	QLOG_INFO() << "ScriptEngineWorker: starting script" << scriptId << ", thread:" << QThread::currentThread();
	mState = starting;
	mScriptId = scriptId;
	emit startedScript(mScriptId);
}

void ScriptEngineWorker::onScriptRequestingToQuit()
{
	if (!mScriptControl.isInEventDrivenMode()) {
		// Somebody erroneously called script.quit() before entering event loop, so we must force event loop for script
		// and only then quit, to send completed() signal properly.
		mScriptControl.run();
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

	Scriptable<BatteryInterface>::registerMetatype(engine);
	Scriptable<ColorSensorInterface>::registerMetatype(engine);
	Scriptable<DisplayInterface>::registerMetatype(engine);
	Scriptable<EncoderInterface>::registerMetatype(engine);
	Scriptable<EventCodeInterface>::registerMetatype(engine);
	Scriptable<EventDeviceInterface>::registerMetatype(engine);
	Scriptable<EventInterface>::registerMetatype(engine);
	Scriptable<GamepadInterface>::registerMetatype(engine);
	Scriptable<GyroSensorInterface>::registerMetatype(engine);
	Scriptable<FifoInterface>::registerMetatype(engine);
	Scriptable<KeysInterface>::registerMetatype(engine);
	Scriptable<LedInterface>::registerMetatype(engine);
	Scriptable<LineSensorInterface>::registerMetatype(engine);
	Scriptable<MailboxInterface>::registerMetatype(engine);
	Scriptable<MotorInterface>::registerMetatype(engine);
	Scriptable<ObjectSensorInterface>::registerMetatype(engine);
	Scriptable<SensorInterface>::registerMetatype(engine);
	Scriptable<SoundSensorInterface>::registerMetatype(engine);
	Scriptable<QTimer>::registerMetatype(engine);
	qScriptRegisterMetaType(engine, timeValToScriptValue, timeValFromScriptValue);
	Scriptable<VectorSensorInterface>::registerMetatype(engine);

	qScriptRegisterSequenceMetaType<QVector<int>>(engine);
	qScriptRegisterSequenceMetaType<QStringList>(engine);

	engine->globalObject().setProperty("brick", engine->newQObject(&mBrick));
	engine->globalObject().setProperty("script", engine->newQObject(&mScriptControl));

	if (mMailbox) {
		engine->globalObject().setProperty("mailbox", engine->newQObject(mMailbox));
	}

	// Gamepad can still be accessed from script as brick.gamepad(), 'gamepad' variable is here for backwards
	// compatibility.
	if (mBrick.gamepad()) {
		engine->globalObject().setProperty("gamepad", engine->newQObject(mBrick.gamepad()));
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

void ScriptEngineWorker::evalSystemJs(QScriptEngine * const engine) const
{
	const QString systemJsPath = trikKernel::Paths::systemScriptsPath() + "system.js";
	if (QFile::exists(systemJsPath)) {
		engine->evaluate(trikKernel::FileUtils::readFromFile(systemJsPath));
		if (engine->hasUncaughtException()) {
			const int line = engine->uncaughtExceptionLineNumber();
			const QString message = engine->uncaughtException().toString();
			QLOG_ERROR() << "system.js: Uncaught exception at line" << line << ":" << message;
		}
	} else {
		QLOG_ERROR() << "system.js not found, path:" << systemJsPath;
	}

	for (const auto &functionName : mRegisteredUserFunctions.keys()) {
		QScriptValue functionValue = engine->newFunction(mRegisteredUserFunctions[functionName]);
		engine->globalObject().setProperty(functionName, functionValue);
	}
}
