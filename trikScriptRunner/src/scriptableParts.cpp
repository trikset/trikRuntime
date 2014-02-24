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

QScriptValue trikScriptRunner::analogSensorToScriptValue(QScriptEngine *engine, AnalogSensor* const &in)
{
	return engine->newQObject(in);
}

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

QScriptValue trikScriptRunner::keysToScriptValue(QScriptEngine *engine, trikControl::Keys* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::keysFromScriptValue(QScriptValue const &object, trikControl::Keys* &out)
{
	out = qobject_cast<Keys*>(object.toQObject());
}

QScriptValue trikScriptRunner::ledToScriptValue(QScriptEngine *engine, trikControl::Led* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::ledFromScriptValue(QScriptValue const &object, trikControl::Led* &out)
{
	out = qobject_cast<Led*>(object.toQObject());
}

void trikScriptRunner::motorFromScriptValue(QScriptValue const &object, Motor* &out)
{
	out = qobject_cast<Motor*>(object.toQObject());
}

QScriptValue trikScriptRunner::motorToScriptValue(QScriptEngine *engine, Motor* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::analogSensorFromScriptValue(QScriptValue const &object, AnalogSensor* &out)
{
	out = qobject_cast<AnalogSensor*>(object.toQObject());
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
