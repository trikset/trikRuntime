/* Copyright 2013 - 2015 Yurii Litvinov, CyberTech Labs Ltd.
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

#include <trikKernel/fileUtils.h>
#include <trikKernel/paths.h>
#include <trikControl/batteryInterface.h>
#include <trikControl/colorSensorInterface.h>
#include <trikControl/displayInterface.h>
#include <trikControl/encoderInterface.h>
#include <trikControl/lineSensorInterface.h>
#include <trikControl/motorInterface.h>
#include <trikControl/objectSensorInterface.h>
#include <trikControl/soundSensorInterface.h>
#include <trikControl/sensorInterface.h>
#include <trikControl/vectorSensorInterface.h>
#include <trikNetwork/mailboxInterface.h>
#include <trikNetwork/gamepadInterface.h>

#include "scriptableParts.h"
#include "utils.h"

#include <QsLog.h>

using namespace trikScriptRunner;
using namespace trikControl;
using namespace trikNetwork;

Q_DECLARE_METATYPE(Threading*)
Q_DECLARE_METATYPE(ColorSensorInterface*)
Q_DECLARE_METATYPE(BatteryInterface*)
Q_DECLARE_METATYPE(DisplayInterface*)
Q_DECLARE_METATYPE(EncoderInterface*)
Q_DECLARE_METATYPE(GamepadInterface*)
Q_DECLARE_METATYPE(KeysInterface*)
Q_DECLARE_METATYPE(LedInterface*)
Q_DECLARE_METATYPE(LineSensorInterface*)
Q_DECLARE_METATYPE(MailboxInterface*)
Q_DECLARE_METATYPE(MotorInterface*)
Q_DECLARE_METATYPE(ObjectSensorInterface*)
Q_DECLARE_METATYPE(SoundSensorInterface*)
Q_DECLARE_METATYPE(SensorInterface*)
Q_DECLARE_METATYPE(VectorSensorInterface*)
Q_DECLARE_METATYPE(FifoInterface*)
Q_DECLARE_METATYPE(QVector<int>)
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
	engine->evaluate(QString("script.sendMessage(\"%1\");").arg(result));

	return engine->toScriptValue(result);
}

ScriptEngineWorker::ScriptEngineWorker(trikControl::BrickInterface &brick
		, trikNetwork::MailboxInterface * const mailbox
		, trikNetwork::GamepadInterface * const gamepad
		, ScriptExecutionControl &scriptControl
		)
	: mBrick(brick)
	, mMailbox(mailbox)
	, mGamepad(gamepad)
	, mScriptControl(scriptControl)
	, mThreadingVariable(this, scriptControl)
	, mDirectScriptsEngine(nullptr)
	, mScriptId(0)
	, mState(ready)
{
	connect(&mScriptControl, SIGNAL(quitSignal()), this, SLOT(onScriptRequestingToQuit()));

	registerUserFunction("print", print);
}

void ScriptEngineWorker::brickBeep()
{
	mBrick.playSound(trikKernel::Paths::mediaPath() + "media/beep_soft.wav");
}

void ScriptEngineWorker::reset()
{
	if (mState == resetting) {
		return;
	}

	while (mState == starting) {
		QThread::yieldCurrentThread();
	}

	if (mState == ready) {
		/// Engine is ready for execution, but we need to clear brick state before we go.
		QLOG_INFO() << "ScriptEngineWorker: 'soft' reset";
		mState = resetting;
		clearMailboxAndGamepadStateState();
		clearBrickState();
		mState = ready;
		return;
	}

	QLOG_INFO() << "ScriptEngineWorker: reset started";

	mState = resetting;

	mScriptControl.reset();

	clearMailboxAndGamepadStateState();

	mThreadingVariable.reset();

	if (mDirectScriptsEngine) {
		mDirectScriptsEngine->abortEvaluation();
		QLOG_INFO() << "ScriptEngineWorker : ending interpretation";
		emit completed(mDirectScriptsEngine->hasUncaughtException()
				? mDirectScriptsEngine->uncaughtException().toString()
				: "", mScriptId);
		mDirectScriptsEngine->deleteLater();
		mDirectScriptsEngine = nullptr;
	}

	clearBrickState();
	mState = ready;
	QLOG_INFO() << "ScriptEngineWorker: reset complete";
}

void ScriptEngineWorker::run(const QString &script, int scriptId)
{
	startScriptEvaluation(scriptId);
	QMetaObject::invokeMethod(this, "doRun", Q_ARG(const QString &, script));
}

void ScriptEngineWorker::doRun(const QString &script)
{
	mThreadingVariable.startMainThread(script);
	mState = running;
	mThreadingVariable.waitForAll();
	const QString error = mThreadingVariable.errorMessage();
	reset();
	QLOG_INFO() << "ScriptEngineWorker: evaluation ended with message" << error;
	emit completed(error, mScriptId);
}

void ScriptEngineWorker::runDirect(const QString &command, int scriptId)
{
	if (!mScriptControl.isInEventDrivenMode()) {
		QLOG_INFO() << "ScriptEngineWorker: starting interpretation";
		reset();
		startScriptEvaluation(scriptId);
		mDirectScriptsEngine = createScriptEngine(false);
		mScriptControl.run();
		mState = running;
	}

	QMetaObject::invokeMethod(this, "doRunDirect", Q_ARG(const QString &, command));
}

void ScriptEngineWorker::doRunDirect(const QString &command)
{
	if (mDirectScriptsEngine) {
		mDirectScriptsEngine->evaluate(command);
		if (mDirectScriptsEngine->hasUncaughtException()) {
			reset();
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
		// and only then quit, to send properly completed() signal.
		mScriptControl.run();
	}

	reset();
}

QScriptEngine * ScriptEngineWorker::createScriptEngine(bool supportThreads)
{
	QScriptEngine *engine = new QScriptEngine();
	QLOG_INFO() << "New script engine" << engine << ", thread:" << QThread::currentThread();

	qScriptRegisterMetaType(engine, batteryToScriptValue, batteryFromScriptValue);
	qScriptRegisterMetaType(engine, displayToScriptValue, displayFromScriptValue);
	qScriptRegisterMetaType(engine, encoderToScriptValue, encoderFromScriptValue);
	qScriptRegisterMetaType(engine, gamepadToScriptValue, gamepadFromScriptValue);
	qScriptRegisterMetaType(engine, keysToScriptValue, keysFromScriptValue);
	qScriptRegisterMetaType(engine, ledToScriptValue, ledFromScriptValue);
	qScriptRegisterMetaType(engine, mailboxToScriptValue, mailboxFromScriptValue);
	qScriptRegisterMetaType(engine, motorToScriptValue, motorFromScriptValue);
	qScriptRegisterMetaType(engine, sensorToScriptValue, sensorFromScriptValue);
	qScriptRegisterMetaType(engine, vectorSensorToScriptValue, vectorSensorFromScriptValue);
	qScriptRegisterMetaType(engine, lineSensorToScriptValue, lineSensorFromScriptValue);
	qScriptRegisterMetaType(engine, colorSensorToScriptValue, colorSensorFromScriptValue);
	qScriptRegisterMetaType(engine, objectSensorToScriptValue, objectSensorFromScriptValue);
	qScriptRegisterMetaType(engine, soundSensorToScriptValue, soundSensorFromScriptValue);
	qScriptRegisterMetaType(engine, timerToScriptValue, timerFromScriptValue);
	qScriptRegisterMetaType(engine, fifoToScriptValue, fifoFromScriptValue);
	qScriptRegisterSequenceMetaType<QVector<int>>(engine);
	qScriptRegisterSequenceMetaType<QStringList>(engine);

	engine->globalObject().setProperty("brick", engine->newQObject(&mBrick));
	engine->globalObject().setProperty("script", engine->newQObject(&mScriptControl));

	if (mMailbox) {
		engine->globalObject().setProperty("mailbox", engine->newQObject(mMailbox));
	}

	if (mGamepad) {
		engine->globalObject().setProperty("gamepad", engine->newQObject(mGamepad));
	}

	if (supportThreads) {
		engine->globalObject().setProperty("Threading", engine->newQObject(&mThreadingVariable));
	}

	evalSystemJs(engine);

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

void ScriptEngineWorker::clearBrickState()
{
	mBrick.reset();
}

void ScriptEngineWorker::clearMailboxAndGamepadStateState()
{
	if (mMailbox) {
		mMailbox->reset();
	}

	if (mGamepad) {
		mGamepad->reset();
	}
}

