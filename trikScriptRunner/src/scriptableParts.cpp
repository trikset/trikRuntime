/* Copyright 2013 - 2015 Yurii Litvinov and CyberTech Labs Ltd.
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
using namespace trikNetwork;

QScriptValue trikScriptRunner::batteryToScriptValue(QScriptEngine *engine, BatteryInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::batteryFromScriptValue(const QScriptValue &object, BatteryInterface* &out)
{
	out = qobject_cast<BatteryInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::displayToScriptValue(QScriptEngine *engine, DisplayInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::displayFromScriptValue(const QScriptValue &object, DisplayInterface* &out)
{
	out = qobject_cast<DisplayInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::encoderToScriptValue(QScriptEngine *engine, EncoderInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::encoderFromScriptValue(const QScriptValue &object, EncoderInterface* &out)
{
	out = qobject_cast<EncoderInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::eventDeviceToScriptValue(QScriptEngine *engine, EventDeviceInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::eventDeviceFromScriptValue(const QScriptValue &object, EventDeviceInterface* &out)
{
	out = qobject_cast<EventDeviceInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::gamepadToScriptValue(QScriptEngine *engine, GamepadInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::gamepadFromScriptValue(const QScriptValue &object, GamepadInterface* &out)
{
	out = qobject_cast<GamepadInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::keysToScriptValue(QScriptEngine *engine, KeysInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::keysFromScriptValue(const QScriptValue &object, KeysInterface* &out)
{
	out = qobject_cast<KeysInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::ledToScriptValue(QScriptEngine *engine, LedInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::ledFromScriptValue(const QScriptValue &object, LedInterface* &out)
{
	out = qobject_cast<LedInterface*>(object.toQObject());
}

void trikScriptRunner::motorFromScriptValue(const QScriptValue &object, MotorInterface* &out)
{
	out = qobject_cast<MotorInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::motorToScriptValue(QScriptEngine *engine, MotorInterface* const &in)
{
	return engine->newQObject(in);
}

QScriptValue trikScriptRunner::sensorToScriptValue(QScriptEngine *engine, SensorInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::sensorFromScriptValue(const QScriptValue &object, SensorInterface* &out)
{
	out = qobject_cast<SensorInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::vectorSensorToScriptValue(QScriptEngine *engine, VectorSensorInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::vectorSensorFromScriptValue(const QScriptValue &object, VectorSensorInterface* &out)
{
	out = qobject_cast<VectorSensorInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::lineSensorToScriptValue(
		QScriptEngine *engine, trikControl::LineSensorInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::lineSensorFromScriptValue(const QScriptValue &object, LineSensorInterface* &out)
{
	out = qobject_cast<LineSensorInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::colorSensorToScriptValue(
		QScriptEngine *engine, trikControl::ColorSensorInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::colorSensorFromScriptValue(const QScriptValue &object, ColorSensorInterface* &out)
{
	out = qobject_cast<ColorSensorInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::objectSensorToScriptValue(
		QScriptEngine *engine, trikControl::ObjectSensorInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::objectSensorFromScriptValue(const QScriptValue &object, ObjectSensorInterface* &out)
{
	out = qobject_cast<ObjectSensorInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::soundSensorToScriptValue(
		QScriptEngine *engine, trikControl::SoundSensorInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::soundSensorFromScriptValue(QScriptValue const &object, SoundSensorInterface* &out)
{
	out = qobject_cast<SoundSensorInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::timerToScriptValue(QScriptEngine *engine, QTimer* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::timerFromScriptValue(const QScriptValue &object, QTimer* &out)
{
	out = qobject_cast<QTimer*>(object.toQObject());
}

QScriptValue trikScriptRunner::mailboxToScriptValue(QScriptEngine *engine, MailboxInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::mailboxFromScriptValue(const QScriptValue &object, MailboxInterface* &out)
{
	out = qobject_cast<MailboxInterface*>(object.toQObject());
}

QScriptValue trikScriptRunner::fifoToScriptValue(QScriptEngine *engine, FifoInterface* const &in)
{
	return engine->newQObject(in);
}

void trikScriptRunner::fifoFromScriptValue(const QScriptValue &object, FifoInterface* &out)
{
	out = qobject_cast<FifoInterface*>(object.toQObject());
}
