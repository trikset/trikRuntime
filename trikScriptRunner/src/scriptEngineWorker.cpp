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
#include <trikKernel/debug.h>

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

ScriptEngineWorker::ScriptEngineWorker(trikControl::Brick &brick, QString const &startDirPath)
	: mEngine(nullptr)
	, mBrick(brick)
	, mThreadingVariable(*this)
	, mStartDirPath(startDirPath)
{
	connect(&mBrick, SIGNAL(quitSignal()), this, SLOT(onScriptRequestingToQuit()));
}

void ScriptEngineWorker::brickBeep()
{
	mBrick.playSound(mStartDirPath + "media/beep.wav");
}

void ScriptEngineWorker::reset()
{
	Q_ASSERT(mEngine);

	mEngine->abortEvaluation();

	QMetaObject::invokeMethod(this, "resetScriptEngine", Qt::QueuedConnection);

	if (mBrick.isInEventDrivenMode()) {
		mBrick.stop();
		onScriptEvaluated();
	}
}

ScriptEngineWorker &ScriptEngineWorker::clone()
{
	ScriptEngineWorker *result = new ScriptEngineWorker(mBrick, mStartDirPath);
	result->setParent(this);
	result->init();
	QScriptValue globalObject = result->mEngine->globalObject();
	Utils::copyRecursivelyTo(mEngine->globalObject(), globalObject, result->mEngine);
	result->mEngine->setGlobalObject(globalObject);
	return *result;
}

void ScriptEngineWorker::init()
{
	resetScriptEngine();
}

void ScriptEngineWorker::run(QString const &script, bool inEventDrivenMode, QString const &function)
{
	Q_ASSERT(mEngine);

	if (inEventDrivenMode) {
		mBrick.run();
	}

	mThreadingVariable.setCurrentScript(script);
	QRegExp const functionRegexp(QString(
			"(.*%1\\s*=\\s*\\w*\\s*function\\(.*\\).*)|(.*function\\s+%1\\s*\\(.*\\).*)").arg(function));
	bool const needCallFunction = !function.isEmpty() && functionRegexp.exactMatch(script)
			&& !script.trimmed().endsWith(function + "();");

	mBrick.keys()->reset();

	mEngine->evaluate(needCallFunction ? QString("%1\n%2();").arg(script, function) : script);

	if (!mBrick.isInEventDrivenMode()) {
		mBrick.stop();
		if (!dynamic_cast<ScriptEngineWorker *>(parent())) {
			// Only main thread must wait for others
			mThreadingVariable.waitForAll();
		}

		onScriptEvaluated();
		resetScriptEngine();
	}
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
	if (mEngine) {
		mEngine->deleteLater();
	}

	mEngine = new QScriptEngine();

	mBrick.reset();

	qScriptRegisterMetaType(mEngine, batteryToScriptValue, batteryFromScriptValue);
	qScriptRegisterMetaType(mEngine, displayToScriptValue, displayFromScriptValue);
	qScriptRegisterMetaType(mEngine, encoderToScriptValue, encoderFromScriptValue);
	qScriptRegisterMetaType(mEngine, gamepadToScriptValue, gamepadFromScriptValue);
	qScriptRegisterMetaType(mEngine, keysToScriptValue, keysFromScriptValue);
	qScriptRegisterMetaType(mEngine, ledToScriptValue, ledFromScriptValue);
	qScriptRegisterMetaType(mEngine, mailboxToScriptValue, mailboxFromScriptValue);
	qScriptRegisterMetaType(mEngine, motorToScriptValue, motorFromScriptValue);
	qScriptRegisterMetaType(mEngine, sensorToScriptValue, sensorFromScriptValue);
	qScriptRegisterMetaType(mEngine, sensor3dToScriptValue, sensor3dFromScriptValue);
	qScriptRegisterMetaType(mEngine, lineSensorToScriptValue, lineSensorFromScriptValue);
	qScriptRegisterMetaType(mEngine, colorSensorToScriptValue, colorSensorFromScriptValue);
	qScriptRegisterMetaType(mEngine, objectSensorToScriptValue, objectSensorFromScriptValue);
	qScriptRegisterSequenceMetaType<QVector<int>>(mEngine);

	mEngine->globalObject().setProperty("brick", mEngine->newQObject(&mBrick));
	mEngine->globalObject().setProperty("Threading", mEngine->newQObject(&mThreadingVariable));

	if (QFile::exists(mStartDirPath + "system.js")) {
		mEngine->evaluate(trikKernel::FileUtils::readFromFile(mStartDirPath + "system.js"));
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
	}

	emit completed(error);
}
