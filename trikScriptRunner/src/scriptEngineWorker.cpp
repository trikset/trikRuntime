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

ScriptEngineWorker::ScriptEngineWorker(QString const &configFilePath, QString const &startDirPath)
	: mEngine(NULL)
	, mConfigFilePath(configFilePath)
	, mStartDirPath(startDirPath)
	, mGuiThread(*this->thread())
{
}

void ScriptEngineWorker::reset()
{
	Q_ASSERT(mEngine);

	mEngine->abortEvaluation();

	// We need to delete script engine to clear possible connections from inside Qt Script, but we can't do that
	// right now because we can be in mEngine's call stack. Also we can not invoke it directly since we can be in
	// another thread and deleting it on return to that thread's event loop will surely crash worker, and Qt docs
	// are not clear whether deleteLater implementation is aware of that problem.
	QMetaObject::invokeMethod(mEngine, "deleteLater", Qt::QueuedConnection);

	QMetaObject::invokeMethod(this, "init", Qt::QueuedConnection);
}

void ScriptEngineWorker::init()
{
	mBrick.reset(new Brick(mGuiThread, mConfigFilePath, mStartDirPath));
	connect(mBrick.data(), SIGNAL(quitSignal()), this, SLOT(onScriptRequestingToQuit()));

	resetScriptEngine();
}

void ScriptEngineWorker::run(QString const &script)
{
	Q_ASSERT(mEngine);

	runAndReportException(script);

	if (!mBrick->isInEventDrivenMode()) {
		emit completed();
	}
}

void ScriptEngineWorker::onScriptRequestingToQuit()
{
	abort();

	/// @todo Completed will be already sent by run() after abortExecution?
	emit completed();
}

void ScriptEngineWorker::resetScriptEngine()
{
	mEngine = new QScriptEngine();

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

	QScriptValue brickProxy = mEngine->newQObject(mBrick.data());
	mEngine->globalObject().setProperty("brick", brickProxy);

	if (QFile::exists(mStartDirPath + "system.js")) {
		runAndReportException(trikKernel::FileUtils::readFromFile(mStartDirPath + "system.js"));
	}

	mEngine->setProcessEventsInterval(1);
}

void ScriptEngineWorker::runAndReportException(QString const &script)
{
	QScriptValue const result = mEngine->evaluate(script);

	if (mEngine->hasUncaughtException()) {
		int const line = mEngine->uncaughtExceptionLineNumber();

		qDebug() << "uncaught exception at line" << line << ":" << result.toString();
	}
}
