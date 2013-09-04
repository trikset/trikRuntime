#include "runner.h"

#include <QtCore/QDebug>
#include <QtCore/QCoreApplication>

#include <trikControl/servoMotor.h>
#include <trikControl/sensor.h>

#include "scriptableParts.h"

using namespace scriptRunner;
using namespace trikControl;

Q_DECLARE_METATYPE(ServoMotor*)
Q_DECLARE_METATYPE(PowerMotor*)
Q_DECLARE_METATYPE(Sensor*)

Runner::Runner()
{
	mEngine.setProcessEventsInterval(1);

	qScriptRegisterMetaType(&mEngine, motorToScriptValue, motorFromScriptValue);
	qScriptRegisterMetaType(&mEngine, powerMotorToScriptValue, powerMotorFromScriptValue);
	qScriptRegisterMetaType(&mEngine, sensorToScriptValue, sensorFromScriptValue);

	QScriptValue brickProxy = mEngine.newQObject(&mBrick);
	mEngine.globalObject().setProperty("brick", brickProxy);
}

void Runner::run(QString const &script)
{
	QScriptValue const result = mEngine.evaluate(script);

	if (mEngine.hasUncaughtException()) {
		int const line = mEngine.uncaughtExceptionLineNumber();

		qDebug() << "uncaught script exception at line" << line << ":" << result.toString();
	}
}

void Runner::abort()
{
	mEngine.abortEvaluation();
}
