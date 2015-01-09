/* Copyright 2013 - 2014 Yurii Litvinov, CyberTech Labs Ltd.
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

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QVector>

#include <trikKernel/fileUtils.h>

#include <trikControl/batteryInterface.h>
#include <trikControl/colorSensorInterface.h>
#include <trikControl/displayInterface.h>
#include <trikControl/encoderInterface.h>
#include <trikControl/lineSensorInterface.h>
#include <trikControl/mailboxInterface.h>
#include <trikControl/motorInterface.h>
#include <trikControl/objectSensorInterface.h>
#include <trikControl/sensorInterface.h>
#include <trikControl/vectorSensorInterface.h>

#include "scriptableParts.h"
#include "utils.h"

#include <QsLog.h>

using namespace trikScriptRunner;
using namespace trikControl;

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
Q_DECLARE_METATYPE(SensorInterface*)
Q_DECLARE_METATYPE(VectorSensorInterface*)
Q_DECLARE_METATYPE(QVector<int>)
Q_DECLARE_METATYPE(QTimer*)

ScriptEngineWorker::ScriptEngineWorker(trikControl::BrickInterface &brick, QString const &startDirPath)
	: mEngine(nullptr)
	, mBrick(brick)
	, mThreadingVariable(*this)
	, mStartDirPath(startDirPath)
	, mEngineReset(false)
{
	connect(&mBrick, SIGNAL(quitSignal()), this, SLOT(onScriptRequestingToQuit()));
}

void ScriptEngineWorker::brickBeep()
{
	mBrick.playSound(mStartDirPath + "media/beep_soft.wav");
}

void ScriptEngineWorker::reset()
{
	if (!mEngine) {
		QLOG_FATAL() << "ScriptEngine is null on reset";
		Q_ASSERT(false);
	}

	if (thread() == QThread::currentThread() && mEngine->isEvaluating()) {
		// We are in a stack of mEngine->evaluate, so can't wait for it to finish.
		QLOG_INFO() << "ScriptEngineWorker::reset called from inside of a script, current script engine:" << mEngine
		<< ", thread:" << QThread::currentThread();

		/// @todo: remove this sh~.
		mEngine->abortEvaluation(QScriptValue("aborted"));

		// Restart ScriptEngineWorker::reset when engine is aborted.
		QMetaObject::invokeMethod(this, "reset", Qt::QueuedConnection);
		return;
	}

	QLOG_INFO() << "ScriptEngineWorker: reset started, current script engine:" << mEngine
			<< ", thread:" << QThread::currentThread();

	mEngineReset = !mEngine->isEvaluating();

	bool const inEventDrivenMode = mBrick.isInEventDrivenMode();

	emit abortEvaluation();
	mEngine->abortEvaluation(QScriptValue("aborted"));
	mBrick.reset();

	while (!mEngineReset) {
		QThread::yieldCurrentThread();
	}

	if (inEventDrivenMode) {
		onScriptEvaluated();
		resetScriptEngine();
	}

	QLOG_INFO() << "ScriptEngineWorker: reset complete, current script engine:" << mEngine
			<< ", thread:" << QThread::currentThread();
}

ScriptEngineWorker &ScriptEngineWorker::clone()
{
	ScriptEngineWorker *result = new ScriptEngineWorker(mBrick, mStartDirPath);
	result->setParent(this);
	result->init();
	QScriptValue globalObject = result->mEngine->globalObject();
	Utils::copyRecursivelyTo(mEngine->globalObject(), globalObject, result->mEngine);
	result->mEngine->setGlobalObject(globalObject);
	QObject::connect(this, SIGNAL(abortEvaluation()), result, SLOT(reset()), Qt::DirectConnection);
	return *result;
}

void ScriptEngineWorker::init()
{
	resetScriptEngine();
}

void ScriptEngineWorker::run(QString const &script, bool inEventDrivenMode, int scriptId, QString const &function)
{
	if (!mEngine) {
		QLOG_FATAL() << "ScriptEngine is null on run";
		Q_ASSERT(false);
	}

	if (!inEventDrivenMode || !mBrick.isInEventDrivenMode()) {
		QLOG_INFO() << "ScriptEngineWorker: starting script" << scriptId << ", thread:" << QThread::currentThread();
		mScriptId = scriptId;
		emit startedScript(mScriptId);
	}

	if (inEventDrivenMode) {
		mBrick.run();
	}

	mThreadingVariable.setCurrentScript(script);
	QRegExp const functionRegexp(QString(
			"(.*%1\\s*=\\s*\\w*\\s*function\\(.*\\).*)|(.*function\\s+%1\\s*\\(.*\\).*)").arg(function));
	bool const needCallFunction = !function.isEmpty() && functionRegexp.exactMatch(script)
			&& !script.trimmed().endsWith(function + "();");

	mBrick.keys()->reset();

	QLOG_INFO() << "ScriptEngineWorker: evaluating, script:" << mScriptId
			<< ", thread:" << QThread::currentThread();
	mEngine->evaluate(needCallFunction ? QString("%1\n%2();").arg(script, function) : script);
	QLOG_INFO() << "ScriptEngineWorker: evaluation stopped, script:" << mScriptId
			<< ", thread:" << QThread::currentThread();

	if (!mBrick.isInEventDrivenMode()) {
		mBrick.stop();
		if (!dynamic_cast<ScriptEngineWorker *>(parent())) {
			// Only main thread must wait for others
			mThreadingVariable.waitForAll();
		}

		onScriptEvaluated();
		resetScriptEngine();
	}

	mEngineReset = true;
}

void ScriptEngineWorker::onScriptRequestingToQuit()
{
	if (!mBrick.isInEventDrivenMode()) {
		// Somebody erroneously called brick.quit() before entering event loop, so we must force event loop for brick
		// and only then quit, to send properly completed() signal.
		mBrick.run();
	}

	reset();
}

void ScriptEngineWorker::resetScriptEngine()
{
	QLOG_INFO() << "ScriptEngineWorker: resetting script engine" << mEngine
			<< ", thread: " << QThread::currentThread();
	if (mEngine) {
		mEngine->deleteLater();
	}

	mEngine = new QScriptEngine();
	QLOG_INFO() << "ScriptEngineWorker: new script engine" << mEngine << ", thread:" << QThread::currentThread();

	qScriptRegisterMetaType(mEngine, batteryToScriptValue, batteryFromScriptValue);
	qScriptRegisterMetaType(mEngine, displayToScriptValue, displayFromScriptValue);
	qScriptRegisterMetaType(mEngine, encoderToScriptValue, encoderFromScriptValue);
	qScriptRegisterMetaType(mEngine, gamepadToScriptValue, gamepadFromScriptValue);
	qScriptRegisterMetaType(mEngine, keysToScriptValue, keysFromScriptValue);
	qScriptRegisterMetaType(mEngine, ledToScriptValue, ledFromScriptValue);
	qScriptRegisterMetaType(mEngine, mailboxToScriptValue, mailboxFromScriptValue);
	qScriptRegisterMetaType(mEngine, motorToScriptValue, motorFromScriptValue);
	qScriptRegisterMetaType(mEngine, sensorToScriptValue, sensorFromScriptValue);
	qScriptRegisterMetaType(mEngine, vectorSensorToScriptValue, vectorSensorFromScriptValue);
	qScriptRegisterMetaType(mEngine, lineSensorToScriptValue, lineSensorFromScriptValue);
	qScriptRegisterMetaType(mEngine, colorSensorToScriptValue, colorSensorFromScriptValue);
	qScriptRegisterMetaType(mEngine, objectSensorToScriptValue, objectSensorFromScriptValue);
	qScriptRegisterMetaType(mEngine, timerToScriptValue, timerFromScriptValue);
	qScriptRegisterSequenceMetaType<QVector<int>>(mEngine);

	mEngine->globalObject().setProperty("brick", mEngine->newQObject(&mBrick));
	mEngine->globalObject().setProperty("Threading", mEngine->newQObject(&mThreadingVariable));

	if (QFile::exists(mStartDirPath + "system.js")) {
		mEngine->evaluate(trikKernel::FileUtils::readFromFile(mStartDirPath + "system.js"));
		if (mEngine->hasUncaughtException()) {
			int const line = mEngine->uncaughtExceptionLineNumber();
			QString const message = mEngine->uncaughtException().toString();
			qDebug() << "Uncaught exception at line" << line << ":" << message;
		}
	}

	mEngine->setProcessEventsInterval(1);
}

void ScriptEngineWorker::onScriptEvaluated()
{
	QString error;
	if (mEngine->hasUncaughtException()) {
		int const line = mEngine->uncaughtExceptionLineNumber();
		QString const message = mEngine->uncaughtException().toString();
		error = tr("Line %1: %2").arg(QString::number(line), message);
		qDebug() << "Uncaught exception at line" << line << ":" << message;
		QLOG_ERROR() << "Uncaught exception at line" << line << ":" << message;
	}

	emit completed(error, mScriptId);
}
