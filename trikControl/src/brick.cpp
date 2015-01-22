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

#include "brick.h"

#include <QtCore/QDebug>
#include <QtCore/QDateTime>
#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>

#include <trikKernel/configurer.h>

#include "analogSensor.h"
#include "display.h"
#include "powerMotor.h"
#include "digitalSensor.h"
#include "rangeSensor.h"
#include "pwmCapture.h"
#include "encoder.h"
#include "battery.h"
#include "vectorSensor.h"
#include "keys.h"
#include "led.h"
#include "lineSensor.h"
#include "objectSensor.h"
#include "colorSensor.h"
#include "servoMotor.h"

#include "i2cCommunicator.h"

#include <QsLog.h>

using namespace trikControl;

Brick::Brick(QThread &guiThread, QString const &systemConfig, QString const &modelConfig, const QString &startDirPath)
	: mDisplay(new Display(guiThread, startDirPath))
{
	qRegisterMetaType<QVector<int>>("QVector<int>");

	trikKernel::Configurer configurer(systemConfig, modelConfig);

	for (QString const &initScript : configurer.initScripts()) {
		if (::system(initScript.toStdString().c_str()) != 0) {
			QLOG_ERROR() << "Init script failed";
		}
	}

	mI2cCommunicator.reset(new I2cCommunicator(configurer));

	for (QString const &port : configurer.ports()) {
		QString const &deviceClass = configurer.deviceClass(port);
		if (deviceClass == "servoMotor") {
			mServoMotors.insert(port, new ServoMotor(port, configurer));
		} else if (deviceClass == "pwmCapture") {
			mPwmCaptures.insert(port, new PwmCapture(port, configurer));
		} else if (deviceClass == "powerMotor") {
			mPowerMotors.insert(port, new PowerMotor(port, configurer, *mI2cCommunicator));
		} else if (deviceClass == "analogSensor") {
			mAnalogSensors.insert(port, new AnalogSensor(port, configurer, *mI2cCommunicator));
		} else if (deviceClass == "digitalSensor") {
			mDigitalSensors.insert(port, new DigitalSensor(port, configurer));
		} else if (deviceClass == "rangeSensor") {
			mRangeSensors.insert(port, new RangeSensor(port, configurer));
			/// @todo Range sensor shall be turned on only when needed.
			mRangeSensors[port]->init();
		} else if (deviceClass == "encoder") {
			mEncoders.insert(port, new Encoder(port, configurer, *mI2cCommunicator));
		} else if (deviceClass == "lineSensor") {
			mLineSensors.insert(port, new LineSensor(port, configurer));

			/// @todo This will work only in case when there can be only one video sensor launched at a time.
			connect(mLineSensors[port], SIGNAL(stopped()), this, SIGNAL(stopped()));
		} else if (deviceClass == "objectSensor") {
			mObjectSensors.insert(port, new ObjectSensor(port, configurer));

			/// @todo This will work only in case when there can be only one video sensor launched at a time.
			connect(mObjectSensors[port], SIGNAL(stopped()), this, SIGNAL(stopped()));
		} else if (deviceClass == "colorSensor") {
			mColorSensors.insert(port, new ColorSensor(port, configurer));

			/// @todo This will work only in case when there can be only one video sensor launched at a time.
			connect(mColorSensors[port], SIGNAL(stopped()), this, SIGNAL(stopped()));
		}
	}

	mBattery.reset(new Battery(*mI2cCommunicator));

	if (configurer.isEnabled("accelerometer")) {
		mAccelerometer.reset(new VectorSensor("accelerometer", configurer));
	}

	if (configurer.isEnabled("gyroscope")) {
		mGyroscope.reset(new VectorSensor("gyroscope", configurer));
	}

	mKeys.reset(new Keys(configurer));

	mLed.reset(new Led(configurer));

	mPlayWavFileCommand = configurer.attributeByDevice("playWavFile", "command");
	mPlayMp3FileCommand = configurer.attributeByDevice("playMp3File", "command");
}

Brick::~Brick()
{
	qDeleteAll(mServoMotors);
	qDeleteAll(mPwmCaptures);
	qDeleteAll(mPowerMotors);
	qDeleteAll(mEncoders);
	qDeleteAll(mAnalogSensors);
	qDeleteAll(mDigitalSensors);
	qDeleteAll(mRangeSensors);
	qDeleteAll(mLineSensors);
	qDeleteAll(mObjectSensors);
	qDeleteAll(mColorSensors);
}

trikKernel::LazyMainWidget &Brick::graphicsWidget()
{
	return mDisplay->graphicsWidget();
}

void Brick::reset()
{
	stop();
	mKeys->reset();
	mDisplay->clear();

	/// @todo Temporary, we need more carefully init/deinit range sensors.
	for (RangeSensor * const rangeSensor : mRangeSensors.values()) {
		rangeSensor->init();
	}
}

void Brick::playSound(QString const &soundFileName)
{
	QLOG_INFO() << "Playing " << soundFileName;

	QFileInfo const fileInfo(soundFileName);

	QString command;

	if (fileInfo.suffix() == "wav") {
		command = mPlayWavFileCommand.arg(soundFileName);
	} else if (fileInfo.suffix() == "mp3") {
		command = mPlayMp3FileCommand.arg(soundFileName);
	}

	if (command.isEmpty() || ::system(command.toStdString().c_str()) != 0) {
		QLOG_ERROR() << "Play sound failed";
	}
}

void Brick::say(QString const &text)
{
	QStringList args{"-c", "espeak -v russian_test -s 100 \"" + text + "\""};
	QProcess::startDetached("sh", args);
}

void Brick::stop()
{
	QLOG_INFO() << "Stopping brick";

	for (ServoMotor * const servoMotor : mServoMotors.values()) {
		servoMotor->powerOff();
	}

	for (PowerMotor * const powerMotor : mPowerMotors.values()) {
		powerMotor->powerOff();
	}

	mLed->red();
	mDisplay->hide();

	/// @todo: Also be able to stop initializing sensor.
	for (LineSensor * const lineSensor : mLineSensors) {
		if (lineSensor->status() == DeviceInterface::Status::ready) {
			lineSensor->stop();
		}
	}

	for (ColorSensor * const colorSensor : mColorSensors) {
		if (colorSensor->status() == DeviceInterface::Status::ready) {
			colorSensor->stop();
		}
	}

	for (ObjectSensor * const objectSensor : mObjectSensors) {
		if (objectSensor->status() == DeviceInterface::Status::ready) {
			objectSensor->stop();
		}
	}

	for (RangeSensor * const rangeSensor : mRangeSensors.values()) {
		rangeSensor->stop();
	}
}

MotorInterface *Brick::motor(QString const &port)
{
	if (mPowerMotors.contains(port)) {
		return mPowerMotors[port];
	} else if (mServoMotors.contains(port)) {
		return mServoMotors[port];
	} else {
		return nullptr;
	}
}

PwmCaptureInterface *Brick::pwmCapture(QString const &port)
{
	return mPwmCaptures.value(port, nullptr);
}

SensorInterface *Brick::sensor(QString const &port)
{
	if (mAnalogSensors.contains(port)) {
		return mAnalogSensors[port];
	} else if (mDigitalSensors.contains(port)) {
		return mDigitalSensors[port];
	} else if (mRangeSensors.contains(port)) {
		return mRangeSensors[port];
	} else {
		return nullptr;
	}
}

QStringList Brick::motorPorts(MotorInterface::Type type) const
{
	switch (type) {
	case MotorInterface::Type::powerMotor: {
		return mPowerMotors.keys();
	}
	case MotorInterface::Type::servoMotor: {
		return mServoMotors.keys();
	}
	}

	return QStringList();
}

QStringList Brick::pwmCapturePorts() const
{
	return mPwmCaptures.keys();
}

QStringList Brick::sensorPorts(SensorInterface::Type type) const
{
	switch (type) {
	case SensorInterface::Type::analogSensor: {
		return mAnalogSensors.keys();
	}
	case SensorInterface::Type::digitalSensor: {
		return mDigitalSensors.keys() + mRangeSensors.keys();
	}
	case SensorInterface::Type::specialSensor: {
		// Special sensors can not be connected to standard ports, they have their own methods to access them.
		return QStringList();
	}
	}

	return QStringList();
}

EncoderInterface *Brick::encoder(QString const &port)
{
	return mEncoders.value(port, nullptr);
}

BatteryInterface *Brick::battery()
{
	return mBattery.data();
}

VectorSensorInterface *Brick::accelerometer()
{
	return mAccelerometer.data();
}

VectorSensorInterface *Brick::gyroscope()
{
	return mGyroscope.data();
}

LineSensorInterface *Brick::lineSensor(QString const &port)
{
	return mLineSensors.contains(port) ? mLineSensors[port] : nullptr;
}

ColorSensorInterface *Brick::colorSensor(QString const &port)
{
	return mColorSensors.contains(port) ? mColorSensors[port] : nullptr;
}

ObjectSensorInterface *Brick::objectSensor(QString const &port)
{
	return mObjectSensors.contains(port) ? mObjectSensors[port] : nullptr;
}

KeysInterface* Brick::keys()
{
	return mKeys.data();
}

QStringList Brick::encoderPorts() const
{
	return mEncoders.keys();
}

DisplayInterface *Brick::display()
{
	return mDisplay.data();
}

LedInterface *Brick::led()
{
	return mLed.data();
}
