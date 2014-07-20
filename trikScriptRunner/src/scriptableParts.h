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

#pragma once

#include <QtScript/QScriptEngine>

#include <trikControl/battery.h>
#include <trikControl/display.h>
#include <trikControl/encoder.h>
#include <trikControl/gamepad.h>
#include <trikControl/keys.h>
#include <trikControl/led.h>
#include <trikControl/sensor.h>
#include <trikControl/sensor3d.h>
#include <trikControl/motor.h>
#include <trikControl/lineSensor.h>
#include <trikControl/colorSensor.h>
#include <trikControl/objectSensor.h>

namespace trikScriptRunner {

QScriptValue batteryToScriptValue(QScriptEngine *engine, trikControl::Battery* const &in);
void batteryFromScriptValue(QScriptValue const &object, trikControl::Battery* &out);

QScriptValue displayToScriptValue(QScriptEngine *engine, trikControl::Display* const &in);
void displayFromScriptValue(QScriptValue const &object, trikControl::Display* &out);

QScriptValue encoderToScriptValue(QScriptEngine *engine, trikControl::Encoder* const &in);
void encoderFromScriptValue(QScriptValue const &object, trikControl::Encoder* &out);

QScriptValue gamepadToScriptValue(QScriptEngine *engine, trikControl::Gamepad* const &in);
void gamepadFromScriptValue(QScriptValue const &object, trikControl::Gamepad* &out);

QScriptValue keysToScriptValue(QScriptEngine *engine, trikControl::Keys* const &in);
void keysFromScriptValue(QScriptValue const &object, trikControl::Keys* &out);

QScriptValue ledToScriptValue(QScriptEngine *engine, trikControl::Led* const &in);
void ledFromScriptValue(QScriptValue const &object, trikControl::Led* &out);

QScriptValue motorToScriptValue(QScriptEngine *engine, trikControl::Motor* const &in);
void motorFromScriptValue(QScriptValue const &object, trikControl::Motor* &out);

QScriptValue sensorToScriptValue(QScriptEngine *engine, trikControl::Sensor* const &in);
void sensorFromScriptValue(QScriptValue const &object, trikControl::Sensor* &out);

QScriptValue sensor3dToScriptValue(QScriptEngine *engine, trikControl::Sensor3d* const &in);
void sensor3dFromScriptValue(QScriptValue const &object, trikControl::Sensor3d* &out);

QScriptValue lineSensorToScriptValue(QScriptEngine *engine, trikControl::LineSensor* const &in);
void lineSensorFromScriptValue(QScriptValue const &object, trikControl::LineSensor* &out);

QScriptValue colorSensorToScriptValue(QScriptEngine *engine, trikControl::ColorSensor* const &in);
void colorSensorFromScriptValue(QScriptValue const &object, trikControl::ColorSensor* &out);

QScriptValue objectSensorToScriptValue(QScriptEngine *engine, trikControl::ObjectSensor* const &in);
void objectSensorFromScriptValue(QScriptValue const &object, trikControl::ObjectSensor* &out);


}
