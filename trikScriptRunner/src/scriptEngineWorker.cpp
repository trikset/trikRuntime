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

#include <trikControl/battery.h>
#include <trikControl/display.h>
#include <trikControl/encoder.h>
#include <trikControl/powerMotor.h>
#include <trikControl/sensor.h>
#include <trikControl/analogSensor.h>
#include <trikControl/sensor3d.h>
#include <trikControl/servoMotor.h>

#include "scriptableParts.h"

using namespace trikScriptRunner;
using namespace trikControl;

Q_DECLARE_METATYPE(AnalogSensor*)
Q_DECLARE_METATYPE(Battery*)
Q_DECLARE_METATYPE(Display*)
Q_DECLARE_METATYPE(Encoder*)
Q_DECLARE_METATYPE(Keys*)
Q_DECLARE_METATYPE(Led*)
Q_DECLARE_METATYPE(PowerMotor*)
Q_DECLARE_METATYPE(Sensor*)
Q_DECLARE_METATYPE(Sensor3d*)
Q_DECLARE_METATYPE(ServoMotor*)

ScriptEngineWorker::ScriptEngineWorker(QString const &configFilePath)
	: mBrick(*this->thread(), configFilePath)
{
	qScriptRegisterMetaType(&mEngine, analogSensorToScriptValue, analogSensorFromScriptValue);
	qScriptRegisterMetaType(&mEngine, batteryToScriptValue, batteryFromScriptValue);
	qScriptRegisterMetaType(&mEngine, displayToScriptValue, displayFromScriptValue);
	qScriptRegisterMetaType(&mEngine, encoderToScriptValue, encoderFromScriptValue);
	qScriptRegisterMetaType(&mEngine, keysToScriptValue, keysFromScriptValue);
	qScriptRegisterMetaType(&mEngine, ledToScriptValue, ledFromScriptValue);
	qScriptRegisterMetaType(&mEngine, powerMotorToScriptValue, powerMotorFromScriptValue);
	qScriptRegisterMetaType(&mEngine, sensorToScriptValue, sensorFromScriptValue);
	qScriptRegisterMetaType(&mEngine, sensor3dToScriptValue, sensor3dFromScriptValue);
	qScriptRegisterMetaType(&mEngine, servoMotorToScriptValue, servoMotorFromScriptValue);

	QScriptValue brickProxy = mEngine.newQObject(&mBrick);
	mEngine.globalObject().setProperty("brick", brickProxy);

	mEngine.setProcessEventsInterval(1);
}

void ScriptEngineWorker::run(QString const &script)
{
	if (mEngine.isEvaluating()) {
		qDebug() << "Script is already running";

		return;
	}

	QScriptValue const result = mEngine.evaluate(script);

	if (mEngine.hasUncaughtException()) {
		int const line = mEngine.uncaughtExceptionLineNumber();

		qDebug() << "uncaught exception at line" << line << ":" << result.toString();
	}

	if (!mBrick.isInEventDrivenMode()) {
		emit completed();
	}
}

void ScriptEngineWorker::abort()
{
	mEngine.abortEvaluation();
}

bool ScriptEngineWorker::isRunning() const
{
	return mEngine.isEvaluating();
}
