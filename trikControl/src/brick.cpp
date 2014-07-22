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

#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>

#include "angularServoMotor.h"
#include "continiousRotationServoMotor.h"
#include "powerMotor.h"

#include "configurer.h"
#include "i2cCommunicator.h"

using namespace trikControl;

Brick::Brick(QThread &guiThread, QString const &configFilePath, const QString &startDirPath)
	: mConfigurer(new Configurer(configFilePath))
	, mI2cCommunicator(NULL)
	, mDisplay(guiThread, startDirPath)
	, mInEventDrivenMode(false)
{
	if (::system(mConfigurer->initScript().toStdString().c_str()) != 0) {
		qDebug() << "Init script failed";
	}

	mI2cCommunicator = new I2cCommunicator(mConfigurer->i2cPath(), mConfigurer->i2cDeviceId());

	for (QString const &port : mConfigurer->servoMotorPorts()) {
		QString const servoMotorType = mConfigurer->servoMotorDefaultType(port);

		ServoMotor *servoMotor = NULL;
		if (mConfigurer->isServoMotorTypeContiniousRotation(servoMotorType)) {
			servoMotor = new ContiniousRotationServoMotor(
					mConfigurer->servoMotorTypeMin(servoMotorType)
					, mConfigurer->servoMotorTypeMax(servoMotorType)
					, mConfigurer->servoMotorTypeZero(servoMotorType)
					, mConfigurer->servoMotorTypeStop(servoMotorType)
					, mConfigurer->servoMotorDeviceFile(port)
					, mConfigurer->servoMotorPeriodFile(port)
					, mConfigurer->servoMotorPeriod(port)
					, mConfigurer->servoMotorInvert(port)
					);
		} else {
			servoMotor = new AngularServoMotor(
					mConfigurer->servoMotorTypeMin(servoMotorType)
					, mConfigurer->servoMotorTypeMax(servoMotorType)
					, mConfigurer->servoMotorTypeZero(servoMotorType)
					, mConfigurer->servoMotorTypeStop(servoMotorType)
					, mConfigurer->servoMotorDeviceFile(port)
					, mConfigurer->servoMotorPeriodFile(port)
					, mConfigurer->servoMotorPeriod(port)
					, mConfigurer->servoMotorInvert(port)
					);
		}

		mServoMotors.insert(port, servoMotor);
	}

	for (QString const &port : mConfigurer->pwmCapturePorts()) {
		PwmCapture *pwmCapture = new PwmCapture(
				mConfigurer->pwmCaptureFrequencyFile(port)
				, mConfigurer->pwmCaptureDutyFile(port)
				);

		mPwmCaptures.insert(port, pwmCapture);
	}

	for (QString const &port : mConfigurer->powerMotorPorts()) {
		PowerMotor *powerMotor = new PowerMotor(
				*mI2cCommunicator
				, mConfigurer->powerMotorI2cCommandNumber(port)
				, mConfigurer->powerMotorInvert(port)
				);

		mPowerMotors.insert(port, powerMotor);
	}

	for (QString const &port : mConfigurer->analogSensorPorts()) {
		AnalogSensor *analogSensor = new AnalogSensor(
			*mI2cCommunicator
			, mConfigurer->analogSensorI2cCommandNumber(port)
			);

		mAnalogSensors.insert(port, analogSensor);
	}

	for (QString const &port : mConfigurer->digitalSensorPorts()) {
		QString const digitalSensorType = mConfigurer->digitalSensorDefaultType(port);

		DigitalSensor *digitalSensor = new DigitalSensor(
				mConfigurer->digitalSensorTypeMin(digitalSensorType)
				, mConfigurer->digitalSensorTypeMax(digitalSensorType)
				, mConfigurer->digitalSensorDeviceFile(port)
				);

		mDigitalSensors.insert(port, digitalSensor);
	}

	for (QString const &port : mConfigurer->encoderPorts()) {
		Encoder *encoder = new Encoder(*mI2cCommunicator, mConfigurer->encoderI2cCommandNumber(port));
		mEncoders.insert(port, encoder);
	}

	mBattery = new Battery(*mI2cCommunicator);

	if (mConfigurer->hasAccelerometer()) {
		mAccelerometer = new Sensor3d(mConfigurer->accelerometerMin()
				, mConfigurer->accelerometerMax()
				, mConfigurer->accelerometerDeviceFile()
				);
	}

	if (mConfigurer->hasGyroscope()) {
		mGyroscope = new Sensor3d(mConfigurer->gyroscopeMin()
				, mConfigurer->gyroscopeMax()
				, mConfigurer->gyroscopeDeviceFile()
				);
	}

	mKeys = new Keys(mConfigurer->keysDeviceFile());

	mLed = new Led(mConfigurer->ledRedDeviceFile()
			, mConfigurer->ledGreenDeviceFile()
			, mConfigurer->ledOn()
			, mConfigurer->ledOff()
			);

	if (mConfigurer->hasGamepad()) {
		mGamepad = new Gamepad(mConfigurer->gamepadPort());
	}

	if (mConfigurer->hasLineSensor()) {
		mLineSensor = new LineSensor(mConfigurer->lineSensorScript()
				, mConfigurer->lineSensorInFifo()
				, mConfigurer->lineSensorOutFifo()
				, mConfigurer->lineSensorToleranceFactor()
				);
	}

	if (mConfigurer->hasObjectSensor()) {
		mObjectSensor = new ObjectSensor(mConfigurer->objectSensorScript()
				, mConfigurer->objectSensorInFifo()
				, mConfigurer->objectSensorOutFifo()
				, mConfigurer->objectSensorToleranceFactor()
				);
	}

	if (mConfigurer->hasColorSensor()) {
		mColorSensor = new ColorSensor(mConfigurer->colorSensorScript()
				, mConfigurer->colorSensorInFifo()
				, mConfigurer->colorSensorOutFifo()
				, mConfigurer->colorSensorM()
				, mConfigurer->colorSensorN()
				);
	}
}

Brick::~Brick()
{
	delete mConfigurer;
	qDeleteAll(mServoMotors);
	qDeleteAll(mPwmCaptures);
	qDeleteAll(mPowerMotors);
	qDeleteAll(mEncoders);
	qDeleteAll(mAnalogSensors);
	qDeleteAll(mDigitalSensors);
	delete mAccelerometer;
	delete mGyroscope;
	delete mBattery;
	delete mI2cCommunicator;
	delete mLed;
	delete mKeys;
	delete mGamepad;
	delete mLineSensor;
	delete mColorSensor;
	delete mObjectSensor;
}

void Brick::reset()
{
	stop();
	mKeys->reset();
	mDisplay.clear();
	mInEventDrivenMode = false;
}

void Brick::playSound(QString const &soundFileName)
{
	qDebug() << soundFileName;

	QFileInfo const fileInfo(soundFileName);

	QString command;

	if (fileInfo.suffix() == "wav") {
		command = mConfigurer->playWavFileCommand().arg(soundFileName);
	} else if (fileInfo.suffix() == "mp3") {
		command = mConfigurer->playMp3FileCommand().arg(soundFileName);
	}

	if (command.isEmpty() || ::system(command.toStdString().c_str()) != 0) {
		qDebug() << "Play sound failed";
	}
}

void Brick::say(QString const &text)
{
	this->system("espeak -v russian_test -s 100 \"" + text + "\"");
}

void Brick::stop()
{
	for (ServoMotor * const servoMotor : mServoMotors.values()) {
		servoMotor->powerOff();
	}

	for (PowerMotor * const powerMotor : mPowerMotors.values()) {
		powerMotor->powerOff();
	}

	mLed->red();
	mDisplay.hide();

	if (mLineSensor) {
		mLineSensor->stop();
	}

	if (mColorSensor) {
		mColorSensor->stop();
	}

	if (mObjectSensor) {
		mObjectSensor->stop();
	}
}

Motor *Brick::motor(QString const &port)
{
	if (mPowerMotors.contains(port)) {
		return mPowerMotors[port];
	} else if (mServoMotors.contains(port)) {
		return mServoMotors[port];
	} else {
		return NULL;
	}
}

PwmCapture *Brick::pwmCapture(QString const &port)
{
	return mPwmCaptures.value(port, NULL);
}

Sensor *Brick::sensor(QString const &port)
{
	if (mAnalogSensors.contains(port)) {
		return mAnalogSensors[port];
	} else if (mDigitalSensors.contains(port)) {
		return mDigitalSensors[port];
	} else {
		return NULL;
	}
}

QStringList Brick::motorPorts(Motor::Type type) const
{
	switch (type) {
		case Motor::powerMotor: {
			return mPowerMotors.keys();
		}
		case Motor::servoMotor: {
			return mServoMotors.keys();
		}
	}

	return QStringList();
}

QStringList Brick::pwmCapturePorts() const
{
	return mPwmCaptures.keys();
}

QStringList Brick::sensorPorts(Sensor::Type type) const
{
	switch (type) {
		case Sensor::analogSensor: {
			return mAnalogSensors.keys();
		}
		case Sensor::digitalSensor: {
			return mDigitalSensors.keys();
		}
		case Sensor::specialSensor: {
			// Special sensors can not be connected to standard ports, they have their own methods to access them.
			return QStringList();
		}
	}

	return QStringList();
}

Encoder *Brick::encoder(QString const &port)
{
	return mEncoders.value(port, NULL);
}

Battery *Brick::battery()
{
	return mBattery;
}

Sensor3d *Brick::accelerometer()
{
	return mAccelerometer;
}

Sensor3d *Brick::gyroscope()
{
	return mGyroscope;
}

LineSensor *Brick::lineSensor()
{
	return mLineSensor;
}

ColorSensor *Brick::colorSensor()
{
	return mColorSensor;
}

ObjectSensor *Brick::objectSensor()
{
	return mObjectSensor;
}

Keys* Brick::keys()
{
	return mKeys;
}

QStringList Brick::encoderPorts() const
{
	return mEncoders.keys();
}

Gamepad* Brick::gamepad()
{
	return mGamepad;
}

void Brick::wait(int const &milliseconds) const
{
	SleeperThread::msleep(milliseconds);
}

qint64 Brick::time() const
{
	return QDateTime::currentMSecsSinceEpoch();
}

Display *Brick::display()
{
	return &mDisplay;
}

Led *Brick::led()
{
	return mLed;
}

void Brick::run()
{
	mInEventDrivenMode = true;
}

bool Brick::isInEventDrivenMode() const
{
	return mInEventDrivenMode;
}

void Brick::quit()
{
	emit quitSignal();
}

void Brick::system(QString const &command)
{
	QStringList args{"-c", command};
	qDebug() << "Running:" << "sh" << args;
	QProcess::startDetached("sh", args);
}
