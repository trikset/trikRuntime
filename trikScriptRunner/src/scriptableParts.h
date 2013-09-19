#pragma once

#include <QtScript/QScriptEngine>

#include <trikControl/brick.h>
#include <trikControl/servoMotor.h>
#include <trikControl/powerMotor.h>
#include <trikControl/sensor.h>

namespace scriptRunner {

QScriptValue motorToScriptValue(QScriptEngine *engine, trikControl::ServoMotor* const &in);
void motorFromScriptValue(QScriptValue const &object, trikControl::ServoMotor* &out);

QScriptValue powerMotorToScriptValue(QScriptEngine *engine, trikControl::PowerMotor* const &in);
void powerMotorFromScriptValue(QScriptValue const &object, trikControl::PowerMotor* &out);

QScriptValue sensorToScriptValue(QScriptEngine *engine, trikControl::Sensor* const &in);
void sensorFromScriptValue(QScriptValue const &object, trikControl::Sensor* &out);

}
