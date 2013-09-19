#include "scriptableParts.h"

#include <QtCore/QMetaType>

using namespace trikScriptRunner;
using namespace trikControl;

QScriptValue trikScriptRunner::motorToScriptValue(QScriptEngine *engine, ServoMotor* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::motorFromScriptValue(QScriptValue const &object, ServoMotor *&out)
{
	out = qobject_cast<ServoMotor*>(object.toQObject());
}

QScriptValue trikScriptRunner::powerMotorToScriptValue(QScriptEngine *engine, PowerMotor* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::powerMotorFromScriptValue(QScriptValue const &object, PowerMotor* &out)
{
	out = qobject_cast<PowerMotor*>(object.toQObject());
}

QScriptValue trikScriptRunner::sensorToScriptValue(QScriptEngine *engine, trikControl::Sensor* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::sensorFromScriptValue(QScriptValue const &object, trikControl::Sensor* &out)
{
	out = qobject_cast<Sensor*>(object.toQObject());
}
