/* Copyright 2013 Yurii Litvinov
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

#include <trikKernel/fileUtils.h>
#include <trikControl/battery.h>
#include <trikControl/display.h>
#include <trikControl/encoder.h>
#include <trikControl/motor.h>
#include <trikControl/sensor.h>
#include <trikControl/analogSensor.h>
#include <trikControl/sensor3d.h>
#include <trikControl/cameraLineDetectorSensor.h>

#include "scriptableParts.h"

using namespace trikScriptRunner;
using namespace trikControl;

Q_DECLARE_METATYPE(AnalogSensor*)
Q_DECLARE_METATYPE(Battery*)
Q_DECLARE_METATYPE(Display*)
Q_DECLARE_METATYPE(Encoder*)
Q_DECLARE_METATYPE(Gamepad*)
Q_DECLARE_METATYPE(Keys*)
Q_DECLARE_METATYPE(Led*)
Q_DECLARE_METATYPE(Motor*)
Q_DECLARE_METATYPE(Sensor*)
Q_DECLARE_METATYPE(Sensor3d*)
Q_DECLARE_METATYPE(CameraLineDetectorSensor*)

ScriptEngineWorker::ScriptEngineWorker(QString const &configFilePath, const QString &startDirPath)
	: mEngine(NULL)
	, mBrick(*this->thread(), configFilePath, startDirPath)
	, mStartDirPath(startDirPath)
{
	connect(&mBrick, SIGNAL(quitSignal()), this, SLOT(onScriptRequestingToQuit()));
}

void ScriptEngineWorker::run(QString const &script)
{
	if (mEngine != NULL) {
		if (mEngine->isEvaluating()) {
			mEngine->abortEvaluation();
		}

		// Here we can safely delete mEngine, we're not in its stack. There's no way to call run() from Qt Script.
		delete mEngine;
	}

	initScriptEngine();

	QScriptValue brickProxy = mEngine->newQObject(&mBrick);
	mEngine->globalObject().setProperty("brick", brickProxy);

	if (QFile::exists(mStartDirPath + "system.js")) {
		runAndReportException(trikKernel::FileUtils::readFromFile(mStartDirPath + "system.js"));
	}

	runAndReportException(script);

	if (!mBrick.isInEventDrivenMode()) {
		emit completed();
	}
}

void ScriptEngineWorker::abort()
{
	if (mEngine != NULL) {
		mEngine->abortEvaluation();

		// We need to delete script engine to clear possible connections from inside Qt Script, but we can't do that
		// right now because we can be in mEngine's call stack. Also we can not invoke it directly since we can be in
		// another thread and deleting it on return to that thread's event loop will surely crash worker, and Qt docs
		// are not clear whether deleteLater implementation is aware of that problem.
		QMetaObject::invokeMethod(mEngine, "deleteLater");
	}
}

bool ScriptEngineWorker::isRunning() const
{
	if (mEngine == NULL) {
		return false;
	} else {
		return mEngine->isEvaluating();
	}
}

bool ScriptEngineWorker::isInEventDrivenMode() const
{
	return mBrick.isInEventDrivenMode();
}

void ScriptEngineWorker::onScriptRequestingToQuit()
{
	abort();

	/// @todo Completed will be already sent by run() after abortExecution?
	emit completed();
}

void ScriptEngineWorker::initScriptEngine()
{
	mEngine = new QScriptEngine();

	connect(mEngine, SIGNAL(destroyed()), this, SLOT(onScriptEngineDestroyed()));

	mBrick.reset();

	qScriptRegisterMetaType(mEngine, batteryToScriptValue, batteryFromScriptValue);
	qScriptRegisterMetaType(mEngine, displayToScriptValue, displayFromScriptValue);
	qScriptRegisterMetaType(mEngine, encoderToScriptValue, encoderFromScriptValue);
	qScriptRegisterMetaType(mEngine, gamepadToScriptValue, gamepadFromScriptValue);
	qScriptRegisterMetaType(mEngine, keysToScriptValue, keysFromScriptValue);
	qScriptRegisterMetaType(mEngine, ledToScriptValue, ledFromScriptValue);
	qScriptRegisterMetaType(mEngine, motorToScriptValue, motorFromScriptValue);
	qScriptRegisterMetaType(mEngine, sensorToScriptValue, sensorFromScriptValue);
	qScriptRegisterMetaType(mEngine, sensor3dToScriptValue, sensor3dFromScriptValue);
	qScriptRegisterMetaType(mEngine, cameraLineDetectorSensorToScriptValue, cameraLineDetectorSensorFromScriptValue);

	mEngine->setProcessEventsInterval(1);
}

void ScriptEngineWorker::onScriptEngineDestroyed()
{
	mEngine = NULL;
}

void ScriptEngineWorker::runAndReportException(QString const &script)
{
	QScriptValue const result = mEngine->evaluate(script);

	if (mEngine->hasUncaughtException()) {
		int const line = mEngine->uncaughtExceptionLineNumber();

		qDebug() << "uncaught exception at line" << line << ":" << result.toString();
	}
}
