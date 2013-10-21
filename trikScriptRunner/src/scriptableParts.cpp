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

#include "scriptableParts.h"

#include <QtCore/QMetaType>

using namespace trikScriptRunner;
using namespace trikControl;

QScriptValue trikScriptRunner::batteryToScriptValue(QScriptEngine *engine, trikControl::Battery* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::batteryFromScriptValue(QScriptValue const &object, trikControl::Battery* &out)
{
	out = qobject_cast<Battery*>(object.toQObject());
}

QScriptValue trikScriptRunner::displayToScriptValue(QScriptEngine *engine, trikControl::Display* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::displayFromScriptValue(QScriptValue const &object, trikControl::Display* &out)
{
	out = qobject_cast<Display*>(object.toQObject());
}

QScriptValue trikScriptRunner::encoderToScriptValue(QScriptEngine *engine, trikControl::Encoder* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::encoderFromScriptValue(QScriptValue const &object, trikControl::Encoder* &out)
{
	out = qobject_cast<Encoder*>(object.toQObject());
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

QScriptValue trikScriptRunner::sensor3dToScriptValue(QScriptEngine *engine, trikControl::Sensor3d* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::sensor3dFromScriptValue(QScriptValue const &object, trikControl::Sensor3d* &out)
{
	out = qobject_cast<Sensor3d*>(object.toQObject());
}

QScriptValue trikScriptRunner::servoMotorToScriptValue(QScriptEngine *engine, ServoMotor* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::servoMotorFromScriptValue(QScriptValue const &object, ServoMotor *&out)
{
	out = qobject_cast<ServoMotor*>(object.toQObject());
}
