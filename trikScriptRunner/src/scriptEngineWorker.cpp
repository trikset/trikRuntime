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

#include <trikControl/analogSensor.h>
#include <trikControl/battery.h>
#include <trikControl/colorSensor.h>
#include <trikControl/display.h>
#include <trikControl/encoder.h>
#include <trikControl/lineSensor.h>
#include <trikControl/mailbox.h>
#include <trikControl/motor.h>
#include <trikControl/objectSensor.h>
#include <trikControl/sensor.h>
#include <trikControl/sensor3d.h>

#include "scriptableParts.h"
#include "utils.h"

#include <QsLog.h>

using namespace trikScriptRunner;
using namespace trikControl;

Q_DECLARE_METATYPE(Threading*)
Q_DECLARE_METATYPE(AnalogSensor*)
Q_DECLARE_METATYPE(ColorSensor*)
Q_DECLARE_METATYPE(Battery*)
Q_DECLARE_METATYPE(Display*)
Q_DECLARE_METATYPE(Encoder*)
Q_DECLARE_METATYPE(Gamepad*)
Q_DECLARE_METATYPE(Keys*)
Q_DECLARE_METATYPE(Led*)
Q_DECLARE_METATYPE(LineSensor*)
Q_DECLARE_METATYPE(Mailbox*)
Q_DECLARE_METATYPE(Motor*)
Q_DECLARE_METATYPE(ObjectSensor*)
Q_DECLARE_METATYPE(Sensor*)
Q_DECLARE_METATYPE(Sensor3d*)
Q_DECLARE_METATYPE(QVector<int>)
Q_DECLARE_METATYPE(QTimer*)

ScriptEngineWorker::ScriptEngineWorker(trikControl::Brick &brick, QString const &startDirPath)
	: mBrick(brick)
	, mThreadingVariable(this)
	, mStartDirPath(startDirPath)
{
	connect(&mBrick, SIGNAL(quitSignal()), this, SLOT(onScriptRequestingToQuit()));
}

void ScriptEngineWorker::brickBeep()
{
	mBrick.playSound(mStartDirPath + "media/beep_soft.wav");
}

void ScriptEngineWorker::reset()
{
	QLOG_INFO() << "ScriptEngineWorker: reset started";
	mThreadingVariable.reset();
	mBrick.reset();
	QLOG_INFO() << "ScriptEngineWorker: reset complete";
}

void ScriptEngineWorker::run(QString const &script, int scriptId)
{
	startScriptEvaluation(scriptId);
	mThreadingVariable.startMainThread(script);
	mThreadingVariable.waitForAll();
	QLOG_INFO() << "ScriptEngineWorker: evaluation ended with message" << mThreadingVariable.errorMessage();
	emit completed(mThreadingVariable.errorMessage(), mScriptId);
	reset();
}

void ScriptEngineWorker::runDirect(const QString &command, int scriptId)
{
/*	if (!mBrick.isInEventDrivenMode()) {
		startScriptEvaluation(scriptId);
		mBrick.run();
	}

	mThreadingVariable.startMainThread(initScriptEngine(), command);
	mThreadingVariable.waitForAll();
*/
}

void ScriptEngineWorker::startScriptEvaluation(int scriptId)
{
	QLOG_INFO() << "ScriptEngineWorker: starting script" << scriptId << ", thread:" << QThread::currentThread();
	mScriptId = scriptId;
	emit startedScript(mScriptId);
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

QScriptEngine * ScriptEngineWorker::createScriptEngine()
{
	QScriptEngine *engine = new QScriptEngine();
	QLOG_INFO() << "ScriptEngineWorker: new script engine" << engine << ", thread:" << QThread::currentThread();

	qScriptRegisterMetaType(engine, batteryToScriptValue, batteryFromScriptValue);
	qScriptRegisterMetaType(engine, displayToScriptValue, displayFromScriptValue);
	qScriptRegisterMetaType(engine, encoderToScriptValue, encoderFromScriptValue);
	qScriptRegisterMetaType(engine, gamepadToScriptValue, gamepadFromScriptValue);
	qScriptRegisterMetaType(engine, keysToScriptValue, keysFromScriptValue);
	qScriptRegisterMetaType(engine, ledToScriptValue, ledFromScriptValue);
	qScriptRegisterMetaType(engine, mailboxToScriptValue, mailboxFromScriptValue);
	qScriptRegisterMetaType(engine, motorToScriptValue, motorFromScriptValue);
	qScriptRegisterMetaType(engine, sensorToScriptValue, sensorFromScriptValue);
	qScriptRegisterMetaType(engine, sensor3dToScriptValue, sensor3dFromScriptValue);
	qScriptRegisterMetaType(engine, lineSensorToScriptValue, lineSensorFromScriptValue);
	qScriptRegisterMetaType(engine, colorSensorToScriptValue, colorSensorFromScriptValue);
	qScriptRegisterMetaType(engine, objectSensorToScriptValue, objectSensorFromScriptValue);
	qScriptRegisterMetaType(engine, timerToScriptValue, timerFromScriptValue);
	qScriptRegisterSequenceMetaType<QVector<int>>(engine);

	engine->globalObject().setProperty("brick", engine->newQObject(&mBrick));
	engine->globalObject().setProperty("Threading", engine->newQObject(&mThreadingVariable));

	if (QFile::exists(mStartDirPath + "system.js")) {
		engine->evaluate(trikKernel::FileUtils::readFromFile(mStartDirPath + "system.js"));
		if (engine->hasUncaughtException()) {
			int const line = engine->uncaughtExceptionLineNumber();
			QString const message = engine->uncaughtException().toString();
			qDebug() << "system.js: Uncaught exception at line" << line << ":" << message;
			QLOG_ERROR() << "system.js: Uncaught exception at line" << line << ":" << message;
		}
	}

	engine->setProcessEventsInterval(1);
	return engine;
}
