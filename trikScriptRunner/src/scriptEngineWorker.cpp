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

Q_DECLARE_METATYPE(QVector<uint8_t>)
Q_DECLARE_METATYPE(QVector<int>)
Q_DECLARE_METATYPE(trikKernel::TimeVal)
Q_DECLARE_METATYPE(QTimer*)

#define DECLARE_METATYPE_TEMPLATE(TYPE) \
	Q_DECLARE_METATYPE(TYPE*)

#define REGISTER_METATYPE_FOR_ENGINE(TYPE) \
	Scriptable<TYPE>::registerMetatype(engine);

#define REGISTER_METATYPE(TYPE) \
	qRegisterMetaType<TYPE*>(TYPE::staticMetaObject.className());

/// Here we define a convenient template that registers all devices used in trik.
/// When creating a new device(interface), you should append it to this list.
/// So it lets you write the device just one time rather than append appropriate line to each place
/// that uses devices.
/// ATTENTION: do not forget to append newly created device to this list!
#define REGISTER_DEVICES_WITH_TEMPLATE(TEMPLATE) \
	TEMPLATE(BatteryInterface) \
	TEMPLATE(ColorSensorInterface) \
	TEMPLATE(FifoInterface) \
	TEMPLATE(DisplayInterface) \
	TEMPLATE(EncoderInterface) \
	TEMPLATE(EventCodeInterface) \
	TEMPLATE(EventDeviceInterface) \
	TEMPLATE(EventInterface) \
	TEMPLATE(GamepadInterface) \
	TEMPLATE(GyroSensorInterface) \
	TEMPLATE(KeysInterface) \
	TEMPLATE(LedInterface) \
	TEMPLATE(LineSensorInterface) \
	TEMPLATE(MailboxInterface) \
	TEMPLATE(MarkerInterface) \
	TEMPLATE(MotorInterface) \
	TEMPLATE(ObjectSensorInterface) \
	TEMPLATE(SoundSensorInterface) \
	TEMPLATE(SensorInterface) \
	TEMPLATE(Threading) \
	TEMPLATE(VectorSensorInterface)

REGISTER_DEVICES_WITH_TEMPLATE(DECLARE_METATYPE_TEMPLATE)

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
	}

	return engine->toScriptValue(result);
}

QScriptValue timeInterval(QScriptContext *context, QScriptEngine *engine)
{
	int result = trikKernel::TimeVal::timeInterval(context->argument(0).toInteger(), context->argument(1).toInteger());
	return engine->toScriptValue(result);
}

static inline int32_t getMedian(uint8_t &a, uint8_t &b, uint8_t &c, uint8_t &d)
{
	if (a > b)
		std::swap(a, b);
	if (c > d)
		std::swap(c, d);
	if (a > c)
		std::swap(a, c);
	if (b > d)
		std::swap(b, d);
	return (static_cast<int32_t>(b) + c) >> 1;
}

QScriptValue getPhoto(QScriptContext *context,	QScriptEngine *engine)
{
	const QScriptValue & brickValue = engine->globalObject().property("brick");
	QObject *qObjBrick = brickValue.toQObject();
	if (qObjBrick)
	{
		BrickInterface *brick = qobject_cast<BrickInterface*>(qObjBrick);
		if (brick)
		{
			auto port = context->argumentCount() > 0 ? context->argument(0).toString()
				: QString("/dev/video0");
			QLOG_INFO() << "Calling getStillImage()";
			auto data = brick->getStillImage();
			QList<int32_t> result;
			result.reserve(data.size() / 3); //Repack RGB88 from 3 x uint8_t into int32_t
			constexpr auto IMAGE_WIDTH = 320;
			constexpr auto IMAGE_HEIGHT = 240;
			if (data.size() >= IMAGE_WIDTH * IMAGE_HEIGHT * 3) {
				for(int row = 0; row < IMAGE_HEIGHT; row += 2) {
					for(int col = 0; col < IMAGE_WIDTH; col+=2) {
						auto row1 = &data[(row*IMAGE_WIDTH+col)*3];
						auto row2 = row1 + IMAGE_WIDTH*3;
						auto r1 = row1[0];
						auto g1 = row1[1];
						auto b1 = row1[2];
						auto r2 = row1[3];
						auto g2 = row1[4];
						auto b2 = row1[5];
						auto r3 = row2[0];
						auto g3 = row2[1];
						auto b3 = row2[2];
						auto r4 = row2[3];
						auto g4 = row2[4];
						auto b4 = row2[5];

						result.push_back((getMedian(r1, r2, r3, r4) << 16)
							| (getMedian(g1, g2, g3, g4) << 8)
							| getMedian(b1, b2, b3, b4));
					}
				}
			}

			QLOG_INFO() << "Constructed result of getStillImage()";
			auto val = engine->toScriptValue(result);
			QLOG_INFO() << "Result of getStillImage() converted to JS value";
			return val;
		}
		else
		{
			QLOG_ERROR() << "script getPhoto failed at downcasting qObject to Brick";
			return QScriptValue();
		}
	}
	else
	{
		QLOG_ERROR() << "script getPhoto failed to get brick Obj";
		return QScriptValue();
	}

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
	connect(this, SIGNAL(getVariables(QString)), &mThreading, SIGNAL(getVariables(QString)));
	connect(&mThreading, SIGNAL(variablesReady(QJsonObject)), this, SIGNAL(variablesReady(QJsonObject)));

	registerUserFunction("print", print);
	registerUserFunction("timeInterval", timeInterval);
	registerUserFunction("getPhoto", getPhoto);

	REGISTER_DEVICES_WITH_TEMPLATE(REGISTER_METATYPE)
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

QStringList ScriptEngineWorker::knownMethodNames() const
{
	QSet<QString> result = {"brick", "script", "threading"};
	collectMethodNames(result, mBrick.metaObject());
	collectMethodNames(result, mScriptControl.metaObject());
	if (mMailbox) {
		result.insert("mailbox");
		collectMethodNames(result, mMailbox->metaObject());
	}
	collectMethodNames(result, mThreading.metaObject());
	return result.toList();
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


	REGISTER_DEVICES_WITH_TEMPLATE(REGISTER_METATYPE_FOR_ENGINE)

	Scriptable<QTimer>::registerMetatype(engine);
	qScriptRegisterMetaType(engine, timeValToScriptValue, timeValFromScriptValue);
	qScriptRegisterSequenceMetaType<QVector<int>>(engine);
	qScriptRegisterSequenceMetaType<QStringList>(engine);
	qScriptRegisterSequenceMetaType<QVector<uint8_t>>(engine);

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

void ScriptEngineWorker::collectMethodNames(QSet<QString> &result, const QMetaObject *obj) const
{
	for (int i = obj->methodOffset(); i < obj->methodCount(); ++i) {
		const QMetaMethod metaMethod = obj->method(i);
		const QString methodName = QString::fromLatin1(metaMethod.name());
		result.insert(methodName);

		QString methodReturnType = QString::fromLatin1(metaMethod.typeName());
		if (methodReturnType.endsWith('*')) {
			methodReturnType.chop(1);
		}

		const int typeId = QMetaType::type(methodReturnType.toLatin1());
		const QMetaObject *newObj = QMetaType::metaObjectForType(typeId);
		if (newObj) {
			collectMethodNames(result, newObj);
		}
	}
}
