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
#include <QtCore/QDateTime>

#include "configurer.h"
#include "i2cCommunicator.h"

using namespace trikControl;

Brick::Brick()
	: mConfigurer(new Configurer())
	, mI2cCommunicator(NULL)
{
	system(mConfigurer->initScript().toStdString().c_str());

	mI2cCommunicator = new I2cCommunicator(mConfigurer->i2cPath(), mConfigurer->i2cDeviceId());

	foreach (QString const &port, mConfigurer->servoMotorPorts()) {
		QString const motorType = mConfigurer->servoMotorDefaultType(port);

		ServoMotor *servoMotor = new ServoMotor(
				mConfigurer->motorTypeMin(motorType)
				, mConfigurer->motorTypeMax(motorType)
				, mConfigurer->motorTypeZero(motorType)
				, mConfigurer->motorTypeStop(motorType)
				, mConfigurer->servoMotorDeviceFile(port)
				, mConfigurer->servoMotorInvert(port)
				);

		mServoMotors.insert(port, servoMotor);
	}

	foreach (QString const &port, mConfigurer->powerMotorPorts()) {
		PowerMotor *powerMotor = new PowerMotor(
				*mI2cCommunicator
				, mConfigurer->powerMotorI2cCommandNumber(port)
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
	delete mI2cCommunicator;
}

void Brick::playSound(QString const &soundFileName)
{
	QString const command = mConfigurer->playSoundCommand().arg(soundFileName);
	system(command.toStdString().c_str());
}

void Brick::stop()
{
	foreach (ServoMotor * const servoMotor, mServoMotors.values()) {
		servoMotor->powerOff();
	}

	foreach (PowerMotor * const powerMotor, mPowerMotors.values()) {
		powerMotor->powerOff();
	}
}

ServoMotor *Brick::servoMotor(QString const &port)
{
	if (mServoMotors.contains(port)) {
		return mServoMotors.value(port);
	}

	return NULL;
}

PowerMotor *Brick::powerMotor(QString const &port)
{
	if (mPowerMotors.contains(port)) {
		return mPowerMotors.value(port);
	}

	return NULL;
}

Sensor *Brick::sensor(QString const &port)
{
	if (mSensors.contains(port)) {
		return mSensors.value(port);
	}

	return NULL;
}

void Brick::wait(int const &milliseconds) const
{
	SleeperThread::msleep(milliseconds);
}

qint64 Brick::time() const
{
	return QDateTime::currentMSecsSinceEpoch();
}
