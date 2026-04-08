/* Copyright 2024 Daniel Chehade.
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
#include "controller.h"
#include "motors.h"
#include "sensors.h"
#include <QObject>
#include <QQmlApplicationEngine>
#include <QtQml/qqml.h>

namespace trikGui {

/// The widget that is responsible for the device testing page.
class TestingManager : public QObject
{
	Q_OBJECT
	QML_NAMED_ELEMENT(Type)
	QML_UNCREATABLE("Use TestingManager context property")
public:
	/// Constructor
	explicit TestingManager(Controller &controller, QQmlApplicationEngine *engine, QObject *parent = nullptr);
	~TestingManager();

	enum class MotorType {
		PowerMotor = static_cast<int>(Motors::MotorType::PowerMotor),
		ServoMotor = static_cast<int>(Motors::MotorType::ServoMotor),
	};
	Q_ENUM(MotorType)

	enum class SensorType {
		Sensors = static_cast<int>(Sensors::SensorType::Sensors),
		Encoder = static_cast<int>(Sensors::SensorType::Encoder),
		Gyroscope = static_cast<int>(Sensors::SensorType::Gyroscope),
		Accelerometer = static_cast<int>(Sensors::SensorType::Accelerometer),
		Camera = static_cast<int>(Sensors::SensorType::Camera),
		PwmCapture = static_cast<int>(Sensors::SensorType::PwmCapture),
	};
	Q_ENUM(SensorType)

	Q_INVOKABLE void createSensors(SensorType type);

	Q_INVOKABLE void createMotors(MotorType type);

	Q_INVOKABLE void setQmlParent(QObject *parent);

	Q_INVOKABLE bool checkPwmCapture();

	Q_INVOKABLE void configureVideoModule();

private:
	Controller &mController;
	QQmlApplicationEngine *mEngine;
};
} // namespace trikGui
