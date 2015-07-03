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

#pragma once

#include <QtScript/QScriptEngine>
#include <QtCore/QTimer>

#include <trikControl/batteryInterface.h>
#include <trikControl/colorSensorInterface.h>
#include <trikControl/displayInterface.h>
#include <trikControl/encoderInterface.h>
#include <trikControl/keysInterface.h>
#include <trikControl/ledInterface.h>
#include <trikControl/lineSensorInterface.h>
#include <trikControl/motorInterface.h>
#include <trikControl/objectSensorInterface.h>
#include <trikControl/soundSensorInterface.h>
#include <trikControl/sensorInterface.h>
#include <trikControl/vectorSensorInterface.h>

#include <trikNetwork/gamepadInterface.h>
#include <trikNetwork/mailboxInterface.h>

namespace trikScriptRunner {

QScriptValue batteryToScriptValue(QScriptEngine *engine, trikControl::BatteryInterface* const &in);
void batteryFromScriptValue(QScriptValue const &object, trikControl::BatteryInterface* &out);

QScriptValue colorSensorToScriptValue(QScriptEngine *engine, trikControl::ColorSensorInterface* const &in);
void colorSensorFromScriptValue(QScriptValue const &object, trikControl::ColorSensorInterface* &out);

QScriptValue displayToScriptValue(QScriptEngine *engine, trikControl::DisplayInterface* const &in);
void displayFromScriptValue(QScriptValue const &object, trikControl::DisplayInterface* &out);

QScriptValue encoderToScriptValue(QScriptEngine *engine, trikControl::EncoderInterface* const &in);
void encoderFromScriptValue(QScriptValue const &object, trikControl::EncoderInterface* &out);

QScriptValue gamepadToScriptValue(QScriptEngine *engine, trikNetwork::GamepadInterface* const &in);
void gamepadFromScriptValue(QScriptValue const &object, trikNetwork::GamepadInterface* &out);

QScriptValue keysToScriptValue(QScriptEngine *engine, trikControl::KeysInterface* const &in);
void keysFromScriptValue(QScriptValue const &object, trikControl::KeysInterface* &out);

QScriptValue ledToScriptValue(QScriptEngine *engine, trikControl::LedInterface* const &in);
void ledFromScriptValue(QScriptValue const &object, trikControl::LedInterface* &out);

QScriptValue lineSensorToScriptValue(QScriptEngine *engine, trikControl::LineSensorInterface* const &in);
void lineSensorFromScriptValue(QScriptValue const &object, trikControl::LineSensorInterface* &out);

QScriptValue mailboxToScriptValue(QScriptEngine *engine, trikNetwork::MailboxInterface* const &in);
void mailboxFromScriptValue(QScriptValue const &object, trikNetwork::MailboxInterface* &out);

QScriptValue motorToScriptValue(QScriptEngine *engine, trikControl::MotorInterface* const &in);
void motorFromScriptValue(QScriptValue const &object, trikControl::MotorInterface* &out);

QScriptValue objectSensorToScriptValue(QScriptEngine *engine, trikControl::ObjectSensorInterface* const &in);
void objectSensorFromScriptValue(QScriptValue const &object, trikControl::ObjectSensorInterface* &out);

QScriptValue soundSensorToScriptValue(QScriptEngine *engine, trikControl::SoundSensorInterface* const &in);
void soundSensorFromScriptValue(QScriptValue const &object, trikControl::SoundSensorInterface* &out);

QScriptValue timerToScriptValue(QScriptEngine *engine, QTimer* const &in);
void timerFromScriptValue(QScriptValue const &object, QTimer* &out);

QScriptValue sensorToScriptValue(QScriptEngine *engine, trikControl::SensorInterface* const &in);
void sensorFromScriptValue(QScriptValue const &object, trikControl::SensorInterface* &out);

QScriptValue vectorSensorToScriptValue(QScriptEngine *engine, trikControl::VectorSensorInterface* const &in);
void vectorSensorFromScriptValue(QScriptValue const &object, trikControl::VectorSensorInterface* &out);

}
