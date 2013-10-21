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
#include <trikControl/powerMotor.h>
#include <trikControl/sensor.h>
#include <trikControl/sensor3d.h>
#include <trikControl/servoMotor.h>

namespace trikScriptRunner {

QScriptValue batteryToScriptValue(QScriptEngine *engine, trikControl::Battery* const &in);
void batteryFromScriptValue(QScriptValue const &object, trikControl::Battery* &out);

QScriptValue displayToScriptValue(QScriptEngine *engine, trikControl::Display* const &in);
void displayFromScriptValue(QScriptValue const &object, trikControl::Display* &out);

QScriptValue encoderToScriptValue(QScriptEngine *engine, trikControl::Encoder* const &in);
void encoderFromScriptValue(QScriptValue const &object, trikControl::Encoder* &out);

QScriptValue powerMotorToScriptValue(QScriptEngine *engine, trikControl::PowerMotor* const &in);
void powerMotorFromScriptValue(QScriptValue const &object, trikControl::PowerMotor* &out);

QScriptValue sensorToScriptValue(QScriptEngine *engine, trikControl::Sensor* const &in);
void sensorFromScriptValue(QScriptValue const &object, trikControl::Sensor* &out);

QScriptValue sensor3dToScriptValue(QScriptEngine *engine, trikControl::Sensor3d* const &in);
void sensor3dFromScriptValue(QScriptValue const &object, trikControl::Sensor3d* &out);

QScriptValue servoMotorToScriptValue(QScriptEngine *engine, trikControl::ServoMotor* const &in);
void servoMotorFromScriptValue(QScriptValue const &object, trikControl::ServoMotor* &out);

}
