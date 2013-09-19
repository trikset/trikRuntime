#include "scriptEngineWorker.h"

#include <QtCore/QDebug>

#include <trikControl/powerMotor.h>
#include <trikControl/servoMotor.h>
#include <trikControl/sensor.h>

#include "scriptableParts.h"

using namespace scriptRunner;
using namespace trikControl;

Q_DECLARE_METATYPE(ServoMotor*)
Q_DECLARE_METATYPE(PowerMotor*)
Q_DECLARE_METATYPE(Sensor*)

ScriptEngineWorker::ScriptEngineWorker()
{
	qScriptRegisterMetaType(&mEngine, motorToScriptValue, motorFromScriptValue);
	qScriptRegisterMetaType(&mEngine, powerMotorToScriptValue, powerMotorFromScriptValue);
	qScriptRegisterMetaType(&mEngine, sensorToScriptValue, sensorFromScriptValue);

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
}

void ScriptEngineWorker::abort()
{
	mEngine.abortEvaluation();
}

void ScriptEngineWorker::deleteWorker()
{
	deleteLater();
	thread()->quit();
}

bool ScriptEngineWorker::isRunning() const
{
	return mEngine.isEvaluating();
}
