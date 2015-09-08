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
#include <trikControl/fifoInterface.h>

#include <trikNetwork/gamepadInterface.h>
#include <trikNetwork/mailboxInterface.h>

namespace trikScriptRunner {

QScriptValue batteryToScriptValue(QScriptEngine *engine, trikControl::BatteryInterface* const &in);
void batteryFromScriptValue(const QScriptValue &object, trikControl::BatteryInterface* &out);

QScriptValue colorSensorToScriptValue(QScriptEngine *engine, trikControl::ColorSensorInterface* const &in);
void colorSensorFromScriptValue(const QScriptValue &object, trikControl::ColorSensorInterface* &out);

QScriptValue displayToScriptValue(QScriptEngine *engine, trikControl::DisplayInterface* const &in);
void displayFromScriptValue(const QScriptValue &object, trikControl::DisplayInterface* &out);

QScriptValue encoderToScriptValue(QScriptEngine *engine, trikControl::EncoderInterface* const &in);
void encoderFromScriptValue(const QScriptValue &object, trikControl::EncoderInterface* &out);

QScriptValue gamepadToScriptValue(QScriptEngine *engine, trikNetwork::GamepadInterface* const &in);
void gamepadFromScriptValue(const QScriptValue &object, trikNetwork::GamepadInterface* &out);

QScriptValue keysToScriptValue(QScriptEngine *engine, trikControl::KeysInterface* const &in);
void keysFromScriptValue(const QScriptValue &object, trikControl::KeysInterface* &out);

QScriptValue ledToScriptValue(QScriptEngine *engine, trikControl::LedInterface* const &in);
void ledFromScriptValue(const QScriptValue &object, trikControl::LedInterface* &out);

QScriptValue lineSensorToScriptValue(QScriptEngine *engine, trikControl::LineSensorInterface* const &in);
void lineSensorFromScriptValue(const QScriptValue &object, trikControl::LineSensorInterface* &out);

QScriptValue mailboxToScriptValue(QScriptEngine *engine, trikNetwork::MailboxInterface* const &in);
void mailboxFromScriptValue(const QScriptValue &object, trikNetwork::MailboxInterface* &out);

QScriptValue motorToScriptValue(QScriptEngine *engine, trikControl::MotorInterface* const &in);
void motorFromScriptValue(const QScriptValue &object, trikControl::MotorInterface* &out);

QScriptValue objectSensorToScriptValue(QScriptEngine *engine, trikControl::ObjectSensorInterface* const &in);
void objectSensorFromScriptValue(const QScriptValue &object, trikControl::ObjectSensorInterface* &out);

QScriptValue soundSensorToScriptValue(QScriptEngine *engine, trikControl::SoundSensorInterface* const &in);
void soundSensorFromScriptValue(QScriptValue const &object, trikControl::SoundSensorInterface* &out);

QScriptValue timerToScriptValue(QScriptEngine *engine, QTimer* const &in);
void timerFromScriptValue(const QScriptValue &object, QTimer* &out);

QScriptValue sensorToScriptValue(QScriptEngine *engine, trikControl::SensorInterface* const &in);
void sensorFromScriptValue(const QScriptValue &object, trikControl::SensorInterface* &out);

QScriptValue vectorSensorToScriptValue(QScriptEngine *engine, trikControl::VectorSensorInterface* const &in);
void vectorSensorFromScriptValue(const QScriptValue &object, trikControl::VectorSensorInterface* &out);

QScriptValue fifoToScriptValue(QScriptEngine *engine, trikControl::FifoInterface* const &in);
void fifoFromScriptValue(const QScriptValue &object, trikControl::FifoInterface* &out);

}
