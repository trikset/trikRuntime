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

	if (auto script = qobject_cast<ScriptExecutionControl*> (scriptValue.toQObject())) {
		auto msg = QString("print: %1").arg(result);
		QMetaObject::invokeMethod(script, [script, msg](){script->textInStdOut(msg);});
	}

	return engine->toScriptValue(result);
}

QScriptValue timeInterval(QScriptContext *context, QScriptEngine *engine)
{
	int result = trikKernel::TimeVal::timeInterval(context->argument(0).toInt32(), context->argument(1).toInt32());
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
		auto *brick = qobject_cast<trikControl::BrickInterface*>(qObjBrick);
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
	connect(&mScriptControl, &ScriptExecutionControl::quitSignal,
		this, &ScriptEngineWorker::onScriptRequestingToQuit);
	connect(this, &ScriptEngineWorker::getVariables, &mThreading, &Threading::getVariables);
	connect(&mThreading, &Threading::variablesReady, this,&ScriptEngineWorker::variablesReady);

	registerUserFunction("print", print);
	registerUserFunction("timeInterval", timeInterval);
	registerUserFunction("getPhoto", getPhoto);
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

	QMetaObject::invokeMethod(&mThreading, "reset", Qt::QueuedConnection);

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
	QMetaObject::invokeMethod(this, "doRun", Q_ARG(QString, script));
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
	QMutexLocker locker(&mScriptStateMutex);
	if (!mScriptControl.isInEventDrivenMode()) {
		QLOG_INFO() << "ScriptEngineWorker: starting interpretation";
		locker.unlock();
		stopScript();
	}

	QMetaObject::invokeMethod(this, "doRunDirect", Q_ARG(QString, command), Q_ARG(int, scriptId));
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
	REGISTER_METATYPE_FOR_ENGINE(trikScriptRunner::Threading)

	Scriptable<QTimer>::registerMetatype(engine);
	qScriptRegisterMetaType(engine, &timeValToScriptValue, &timeValFromScriptValue);
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
	if (QFileInfo::exists(systemJsPath)) {
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

QStringList ScriptEngineWorker::knownMethodNames() const
{
	QSet<QString> result = {"brick", "script", "threading"};
	TrikScriptRunnerInterface::Helper::collectMethodNames(result, &trikControl::BrickInterface::staticMetaObject);
	TrikScriptRunnerInterface::Helper::collectMethodNames(result, mScriptControl.metaObject());
	if (mMailbox) {
		result.insert("mailbox");
		TrikScriptRunnerInterface::Helper::collectMethodNames(result, mMailbox->metaObject());
	}
	TrikScriptRunnerInterface::Helper::collectMethodNames(result, mThreading.metaObject());
	return result.toList();
}
