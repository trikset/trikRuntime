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

#include <QtCore/QDateTime>
#include <QtCore/QCoreApplication>
#include <QtCore/QProcess>
#include <QtCore/QFileInfo>

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
#include "fifo.h"

#include "i2cCommunicator.h"
#include "moduleLoader.h"

#include <QsLog.h>

using namespace trikControl;

Brick::Brick(const QString &systemConfig, const QString &modelConfig, const QString &startDirPath)
	: mDisplay(new Display(startDirPath))
	, mConfigurer(systemConfig, modelConfig)
{
	qRegisterMetaType<QVector<int>>("QVector<int>");

	for (const QString &initScript : mConfigurer.initScripts()) {
		if (::system(initScript.toStdString().c_str()) != 0) {
			QLOG_ERROR() << "Init script failed";
		}
	}

	mI2cCommunicator.reset(new I2cCommunicator(mConfigurer));
	mModuleLoader.reset(new ModuleLoader());

	for (const QString &port : mConfigurer.ports()) {
		createDevice(port);
	}

	mBattery.reset(new Battery(*mI2cCommunicator));

	if (mConfigurer.isEnabled("accelerometer")) {
		mAccelerometer.reset(new VectorSensor("accelerometer", mConfigurer));
	}

	if (mConfigurer.isEnabled("gyroscope")) {
		mGyroscope.reset(new VectorSensor("gyroscope", mConfigurer));
	}

	mKeys.reset(new Keys(mConfigurer));

	mLed.reset(new Led(mConfigurer));

	mPlayWavFileCommand = mConfigurer.attributeByDevice("playWavFile", "command");
	mPlayMp3FileCommand = mConfigurer.attributeByDevice("playMp3File", "command");
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
	qDeleteAll(mFifos);
}

DisplayWidgetInterface &Brick::graphicsWidget()
{
	return mDisplay->graphicsWidget();
}

void Brick::configure(const QString &portName, const QString &deviceName)
{
	shutdownDevice(portName);

	mConfigurer.configure(portName, deviceName);

	createDevice(portName);
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

void Brick::playSound(const QString &soundFileName)
{
	QLOG_INFO() << "Playing " << soundFileName;

	QFileInfo fileInfo(soundFileName);
	if (fileInfo.isRelative()) {
		fileInfo.setFile("../" + soundFileName);
	}

	QString command;

	if (fileInfo.suffix() == "wav") {
		command = mPlayWavFileCommand.arg(fileInfo.absoluteFilePath());
	} else if (fileInfo.suffix() == "mp3") {
		command = mPlayMp3FileCommand.arg(fileInfo.absoluteFilePath());
	}

	if (command.isEmpty() || ::system(command.toStdString().c_str()) != 0) {
		QLOG_ERROR() << "Play sound failed";
	}
}

void Brick::say(const QString &text)
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

MotorInterface *Brick::motor(const QString &port)
{
	if (mPowerMotors.contains(port)) {
		return mPowerMotors[port];
	} else if (mServoMotors.contains(port)) {
		return mServoMotors[port];
	} else {
		return nullptr;
	}
}

PwmCaptureInterface *Brick::pwmCapture(const QString &port)
{
	return mPwmCaptures.value(port, nullptr);
}

SensorInterface *Brick::sensor(const QString &port)
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

EncoderInterface *Brick::encoder(const QString &port)
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

LineSensorInterface *Brick::lineSensor(const QString &port)
{
	return mLineSensors.contains(port) ? mLineSensors[port] : nullptr;
}

ColorSensorInterface *Brick::colorSensor(const QString &port)
{
	return mColorSensors.contains(port) ? mColorSensors[port] : nullptr;
}

ObjectSensorInterface *Brick::objectSensor(const QString &port)
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

FifoInterface *Brick::fifo(const QString &port)
{
	return mFifos[port];
}

void Brick::shutdownDevice(const QString &port)
{
	const QString &deviceClass = mConfigurer.deviceClass(port);
	if (deviceClass == "servoMotor") {
		mServoMotors[port]->powerOff();
		delete mServoMotors[port];
		mServoMotors.remove(port);
	} else if (deviceClass == "pwmCapture") {
		delete mPwmCaptures[port];
		mPwmCaptures.remove(port);
	} else if (deviceClass == "powerMotor") {
		mPowerMotors[port]->powerOff();
		delete mPowerMotors[port];
		mPowerMotors.remove(port);
	} else if (deviceClass == "analogSensor") {
		delete mAnalogSensors[port];
		mAnalogSensors.remove(port);
	} else if (deviceClass == "digitalSensor") {
		delete mDigitalSensors[port];
		mDigitalSensors.remove(port);
	} else if (deviceClass == "rangeSensor") {
		mRangeSensors[port]->stop();
		delete mRangeSensors[port];
		mRangeSensors.remove(port);
	} else if (deviceClass == "encoder") {
		delete mEncoders[port];
		mEncoders.remove(port);
	} else if (deviceClass == "lineSensor") {
		mLineSensors[port]->stop();
		delete mLineSensors[port];
		mLineSensors.remove(port);
	} else if (deviceClass == "objectSensor") {
		mObjectSensors[port]->stop();
		delete mObjectSensors[port];
		mObjectSensors.remove(port);
	} else if (deviceClass == "colorSensor") {
		mColorSensors[port]->stop();
		delete mColorSensors[port];
		mColorSensors.remove(port);
	} else if (deviceClass == "fifo") {
		delete mFifos[port];
		mFifos.remove(port);
	}
}

void Brick::createDevice(const QString &port)
{
	const QString &deviceClass = mConfigurer.deviceClass(port);
	if (deviceClass == "servoMotor") {
		mServoMotors.insert(port, new ServoMotor(port, mConfigurer));
	} else if (deviceClass == "pwmCapture") {
		mPwmCaptures.insert(port, new PwmCapture(port, mConfigurer));
	} else if (deviceClass == "powerMotor") {
		mPowerMotors.insert(port, new PowerMotor(port, mConfigurer, *mI2cCommunicator));
	} else if (deviceClass == "analogSensor") {
		mAnalogSensors.insert(port, new AnalogSensor(port, mConfigurer, *mI2cCommunicator));
	} else if (deviceClass == "digitalSensor") {
		mDigitalSensors.insert(port, new DigitalSensor(port, mConfigurer));
	} else if (deviceClass == "rangeSensor") {
		mRangeSensors.insert(port, new RangeSensor(port, mConfigurer, *mModuleLoader));
		/// @todo Range sensor shall be turned on only when needed.
		mRangeSensors[port]->init();
	} else if (deviceClass == "encoder") {
		mEncoders.insert(port, new Encoder(port, mConfigurer, *mI2cCommunicator));
	} else if (deviceClass == "lineSensor") {
		mLineSensors.insert(port, new LineSensor(port, mConfigurer));

		/// @todo This will work only in case when there can be only one video sensor launched at a time.
		connect(mLineSensors[port], SIGNAL(stopped()), this, SIGNAL(stopped()));
	} else if (deviceClass == "objectSensor") {
		mObjectSensors.insert(port, new ObjectSensor(port, mConfigurer));
		/// @todo This will work only in case when there can be only one video sensor launched at a time.
		connect(mObjectSensors[port], SIGNAL(stopped()), this, SIGNAL(stopped()));
	} else if (deviceClass == "colorSensor") {
		mColorSensors.insert(port, new ColorSensor(port, mConfigurer));

		/// @todo This will work only in case when there can be only one video sensor launched at a time.
		connect(mColorSensors[port], SIGNAL(stopped()), this, SIGNAL(stopped()));
	} else if (deviceClass == "fifo") {
		mFifos.insert(port, new Fifo(port, mConfigurer));
	}
}
