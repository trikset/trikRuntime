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

#include "brick.h"

#include <QtCore/QSettings>
#include <QtCore/QDebug>

#include "configurer.h"

using namespace trikControl;

Brick::Brick()
	: mConfigurer(new Configurer())
{
	system(mConfigurer->initScript().toStdString().c_str());

	foreach (QString const &port, mConfigurer->servoMotorPorts()) {
		QString const motorType = mConfigurer->servoMotorDefaultType(port);

		ServoMotor *servoMotor = new ServoMotor(
				mConfigurer->motorTypeMin(motorType)
				, mConfigurer->motorTypeMax(motorType)
				, mConfigurer->servoMotorDeviceFile(port)
				, mConfigurer->servoMotorInvert(port)
				);

		mServoMotors.insert(port, servoMotor);
	}

	foreach (QString const &port, mConfigurer->powerMotorPorts()) {
		PowerMotor *powerMotor = new PowerMotor(
				mConfigurer->powerMotorCommand(port)
				, mConfigurer->powerMotorStop(port)
				, mConfigurer->powerMotorInvert(port)
				);

		mPowerMotors.insert(port, powerMotor);
	}

	foreach (QString const &port, mConfigurer->sensorPorts()) {
		QString const sensorType = mConfigurer->sensorDefaultType(port);

		Sensor *sensor = new Sensor(
				mConfigurer->sensorTypeMin(sensorType)
				, mConfigurer->sensorTypeMax(sensorType)
				, mConfigurer->sensorDeviceFile(port)
				);

		mSensors.insert(port, sensor);
	}
}

Brick::~Brick()
{
	delete mConfigurer;
	qDeleteAll(mServoMotors);
	qDeleteAll(mPowerMotors);
	qDeleteAll(mSensors);
}

void Brick::playSound(QString const &soundFileName)
{
	qDebug() << "playSound, file: " << soundFileName;

	QString const command = mConfigurer->playSoundCommand().arg(soundFileName);
	system(command.toStdString().c_str());
}

void Brick::stop()
{
	qDebug() << "stop";

	foreach (ServoMotor * const servoMotor, mServoMotors.values()) {
		servoMotor->powerOff();
	}

	foreach (PowerMotor * const powerMotor, mPowerMotors.values()) {
		powerMotor->powerOff();
	}
}

ServoMotor *Brick::servoMotor(QString const &port)
{
	qDebug() << "motor, port: " << port;

	if (mServoMotors.contains(port)) {
		return mServoMotors.value(port);
	}

	return NULL;
}

PowerMotor *Brick::powerMotor(QString const &port)
{
	qDebug() << "Power motor, port: " << port;

	if (mPowerMotors.contains(port)) {
		return mPowerMotors.value(port);
	}

	return NULL;
}

Sensor *Brick::sensor(QString const &port)
{
	qDebug() << "sensor, port: " << port;

	if (mSensors.contains(port)) {
		return mSensors.value(port);
	}

	return NULL;
}

void Brick::wait(int const &milliseconds) const
{
	SleeperThread::msleep(milliseconds);
}
