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

QScriptValue trikScriptRunner::batteryToScriptValue(QScriptEngine *engine, trikControl::BatteryInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::batteryFromScriptValue(QScriptValue const &object, trikControl::BatteryInterface* &out)
{
	out = qobject_cast<BatteryInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::displayToScriptValue(QScriptEngine *engine, trikControl::DisplayInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::displayFromScriptValue(QScriptValue const &object, trikControl::DisplayInterface* &out)
{
	out = qobject_cast<DisplayInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::encoderToScriptValue(QScriptEngine *engine, trikControl::EncoderInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::encoderFromScriptValue(QScriptValue const &object, trikControl::EncoderInterface* &out)
{
	out = qobject_cast<EncoderInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::gamepadToScriptValue(QScriptEngine *engine, trikControl::GamepadInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::gamepadFromScriptValue(QScriptValue const &object, trikControl::GamepadInterface* &out)
{
	out = qobject_cast<GamepadInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::keysToScriptValue(QScriptEngine *engine, trikControl::KeysInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::keysFromScriptValue(QScriptValue const &object, trikControl::KeysInterface* &out)
{
	out = qobject_cast<KeysInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::ledToScriptValue(QScriptEngine *engine, trikControl::LedInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::ledFromScriptValue(QScriptValue const &object, trikControl::LedInterface* &out)
{
	out = qobject_cast<LedInterface*>(object.toQObject());
}

void trikScriptRunner::motorFromScriptValue(QScriptValue const &object, MotorInterface* &out)
{
	out = qobject_cast<MotorInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::motorToScriptValue(QScriptEngine *engine, MotorInterface* const &in)
{
	return engine->newQObject(in);
}

QScriptValue trikScriptRunner::sensorToScriptValue(QScriptEngine *engine, trikControl::SensorInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::sensorFromScriptValue(QScriptValue const &object, trikControl::SensorInterface* &out)
{
	out = qobject_cast<SensorInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::vectorSensorToScriptValue(QScriptEngine *engine, trikControl::VectorSensorInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::vectorSensorFromScriptValue(QScriptValue const &object, trikControl::VectorSensorInterface* &out)
{
	out = qobject_cast<VectorSensorInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::lineSensorToScriptValue(QScriptEngine *engine, trikControl::LineSensorInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::lineSensorFromScriptValue(QScriptValue const &object, trikControl::LineSensorInterface* &out)
{
	out = qobject_cast<LineSensorInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::colorSensorToScriptValue(QScriptEngine *engine, trikControl::ColorSensorInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::colorSensorFromScriptValue(QScriptValue const &object, trikControl::ColorSensorInterface* &out)
{
	out = qobject_cast<ColorSensorInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::objectSensorToScriptValue(QScriptEngine *engine, trikControl::ObjectSensorInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::objectSensorFromScriptValue(QScriptValue const &object, trikControl::ObjectSensorInterface* &out)
{
	out = qobject_cast<ObjectSensorInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::timerToScriptValue(QScriptEngine *engine, QTimer* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::timerFromScriptValue(QScriptValue const &object, QTimer* &out)
{
	out = qobject_cast<QTimer*>(object.toQObject());
}

QScriptValue trikScriptRunner::mailboxToScriptValue(QScriptEngine *engine, trikControl::MailboxInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::mailboxFromScriptValue(QScriptValue const &object, trikControl::MailboxInterface* &out)
{
	out = qobject_cast<MailboxInterface*>(object.toQObject());
}
