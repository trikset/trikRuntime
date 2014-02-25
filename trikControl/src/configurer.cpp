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

#include "configurer.h"

#include <QtXml/QDomDocument>
#include <QtCore/QFile>
#include <QtCore/QDebug>

using namespace trikControl;

Configurer::Configurer(QString const &configFilePath)
	: mI2cDeviceId(0)
	, mLedOn(0)
	, mLedOff(0)
	, mGamepadPort(0)
{
	QDomDocument config("config");

	QFile file(configFilePath + "config.xml");
	if (!file.open(QIODevice::ReadOnly)) {
		qDebug() << "Failed to open config.xml for reading";
		throw "Failed to open config.xml for reading";
	} if (!config.setContent(&file)) {
		file.close();
		qDebug() << "config.xml parsing failed";
		throw "config.xml parsing failed";
	}

	file.close();

	QDomElement const root = config.documentElement();

	loadInit(root);
	loadServoMotors(root);
	loadPwmCaptures(root);
	loadPowerMotors(root);
	loadAnalogSensors(root);
	loadEncoders(root);
	loadDigitalSensors(root);
	loadServoMotorTypes(root);
	loadDigitalSensorTypes(root);
	loadSound(root);

	mAccelerometer = loadSensor3d(root, "accelerometer");
	mGyroscope = loadSensor3d(root, "gyroscope");

	loadI2c(root);
	loadLed(root);
	loadKeys(root);
	loadGamepadPort(root);
}

QString Configurer::initScript() const
{
	return mInitScript;
}

QStringList Configurer::servoMotorTypes() const
{
	return mServoMotorTypes.keys();
}

QStringList Configurer::digitalSensorTypes() const
{
	return mDigitalSensorTypes.keys();
}

int Configurer::servoMotorTypeMin(QString const &servoMotorType) const
{
	return mServoMotorTypes[servoMotorType].min;
}

int Configurer::servoMotorTypeMax(QString const &servoMotorType) const
{
	return mServoMotorTypes[servoMotorType].max;
}

int Configurer::servoMotorTypeZero(QString const &servoMotorType) const
{
	return mServoMotorTypes[servoMotorType].zero;
}

int Configurer::servoMotorTypeStop(QString const &servoMotorType) const
{
	return mServoMotorTypes[servoMotorType].stop;
}

int Configurer::digitalSensorTypeMin(QString const &digitalSensorType) const
{
	return mDigitalSensorTypes[digitalSensorType].min;
}

int Configurer::digitalSensorTypeMax(QString const &digitalSensorType) const
{
	return mDigitalSensorTypes[digitalSensorType].max;
}

QStringList Configurer::servoMotorPorts() const
{
	return mServoMotorMappings.keys();
}

QString Configurer::servoMotorDeviceFile(QString const &port) const
{
	return mServoMotorMappings[port].deviceFile;
}

QString Configurer::servoMotorPeriodFile(const QString &port) const
{
	return mServoMotorMappings[port].periodFile;
}

int Configurer::servoMotorPeriod(const QString &port) const
{
	return mServoMotorMappings[port].period;
}

QString Configurer::servoMotorDefaultType(QString const &port) const
{
	return mServoMotorMappings[port].defaultType;
}

bool Configurer::servoMotorInvert(QString const &port) const
{
	return mServoMotorMappings[port].invert;
}

QStringList Configurer::pwmCapturePorts() const
{
	return mPwmCaptureMappings.keys();
}

QString Configurer::pwmCaptureFrequencyFile(const QString &port) const
{
	return mPwmCaptureMappings[port].frequencyFile;
}

QString Configurer::pwmCaptureDutyFile(const QString &port) const
{
	return mPwmCaptureMappings[port].dutyFile;
}

QStringList Configurer::powerMotorPorts() const
{
	return mPowerMotorMappings.keys();
}

int Configurer::powerMotorI2cCommandNumber(QString const &port) const
{
	return mPowerMotorMappings[port].i2cCommandNumber;
}

bool Configurer::powerMotorInvert(QString const &port) const
{
	return mPowerMotorMappings[port].invert;
}

QStringList Configurer::analogSensorPorts() const
{
	return mAnalogSensorMappings.keys();
}

int Configurer::analogSensorI2cCommandNumber(QString const &port) const
{
	return mAnalogSensorMappings[port].i2cCommandNumber;
}

QStringList Configurer::encoderPorts() const
{
	return mEncoderMappings.keys();
}

int Configurer::encoderI2cCommandNumber(QString const &port) const
{
	return mEncoderMappings[port].i2cCommandNumber;
}

QStringList Configurer::digitalSensorPorts() const
{
	return mDigitalSensorMappings.keys();
}

QString Configurer::digitalSensorDeviceFile(QString const &port) const
{
	return mDigitalSensorMappings[port].deviceFile;
}

QString Configurer::digitalSensorDefaultType(QString const &port) const
{
	return mDigitalSensorMappings[port].defaultType;
}

QString Configurer::playWavFileCommand() const
{
	return mPlayWavFileCommand;
}

QString Configurer::playMp3FileCommand() const
{
	return mPlayMp3FileCommand;
}

int Configurer::accelerometerMin() const
{
	return mAccelerometer.min;
}

int Configurer::accelerometerMax() const
{
	return mAccelerometer.max;
}

QString Configurer::accelerometerDeviceFile() const
{
	return mAccelerometer.deviceFile;
}

int Configurer::gyroscopeMin() const
{
	return mGyroscope.min;
}

int Configurer::gyroscopeMax() const
{
	return mGyroscope.max;
}

QString Configurer::gyroscopeDeviceFile() const
{
	return mGyroscope.deviceFile;
}

QString Configurer::i2cPath() const
{
	return mI2cPath;
}

int Configurer::i2cDeviceId() const
{
	return mI2cDeviceId;
}

QString Configurer::ledRedDeviceFile() const
{
	return mLedRedDeviceFile;
}

QString Configurer::ledGreenDeviceFile() const
{
	return mLedGreenDeviceFile;
}

int Configurer::ledOn() const
{
	return mLedOn;
}

int Configurer::ledOff() const
{
	return mLedOff;
}

QString Configurer::keysDeviceFile() const
{
	return mKeysDeviceFile;
}

int Configurer::gamepadPort() const
{
	return mGamepadPort;
}

void Configurer::loadInit(QDomElement const &root)
{
	if (root.elementsByTagName("initScript").isEmpty()) {
		qDebug() << "config.xml does not have <initScript> tag";
		throw "config.xml parsing failed";
	}

	QDomElement const init = root.elementsByTagName("initScript").at(0).toElement();
	mInitScript = init.text();
}

void Configurer::loadServoMotors(QDomElement const &root)
{
	if (root.elementsByTagName("servoMotors").isEmpty()) {
		qDebug() << "config.xml does not have <servoMotors> tag";
		throw "config.xml parsing failed";
	}

	QDomElement const servoMotors = root.elementsByTagName("servoMotors").at(0).toElement();
	for (QDomNode child = servoMotors.firstChild()
			; !child.isNull()
			; child = child.nextSibling())
	{
		if (!child.isElement()) {
			continue;
		}

		QDomElement const childElement = child.toElement();
		if (childElement.nodeName() != "servoMotor") {
			qDebug() << "Malformed <servoMotors> tag";
			throw "config.xml parsing failed";
		}

		ServoMotorMapping mapping;
		mapping.port = childElement.attribute("port");
		mapping.deviceFile = childElement.attribute("deviceFile");
		mapping.periodFile = childElement.attribute("periodFile");
		mapping.period = childElement.attribute("period").toInt();
		mapping.defaultType = childElement.attribute("defaultType");
		mapping.invert = childElement.attribute("invert") == "true";

		mServoMotorMappings.insert(mapping.port, mapping);
	}
}

void Configurer::loadPwmCaptures(QDomElement const &root)
{
	if (root.elementsByTagName("pwmCaptures").isEmpty()) {
		qDebug() << "config.xml does not have <pwmCaptures> tag";
		throw "config.xml parsing failed";
	}

	QDomElement const pwmCaptures = root.elementsByTagName("pwmCaptures").at(0).toElement();
	for (QDomNode child = pwmCaptures.firstChild()
			; !child.isNull()
			; child = child.nextSibling())
	{
		if (!child.isElement()) {
			continue;
		}

		QDomElement const childElement = child.toElement();
		if (childElement.nodeName() != "capture") {
			qDebug() << "Malformed <pwmCaptures> tag";
			throw "config.xml parsing failed";
		}

		PwmCaptureMapping mapping;
		mapping.port = childElement.attribute("port");
		mapping.frequencyFile = childElement.attribute("frequencyFile");
		mapping.dutyFile = childElement.attribute("dutyFile");

		mPwmCaptureMappings.insert(mapping.port, mapping);
	}
}

void Configurer::loadPowerMotors(QDomElement const &root)
{
	if (root.elementsByTagName("powerMotors").isEmpty()) {
		qDebug() << "config.xml does not have <powerMotors> tag";
		throw "config.xml parsing failed";
	}

	QDomElement const powerMotors = root.elementsByTagName("powerMotors").at(0).toElement();
	for (QDomNode child = powerMotors.firstChild()
			; !child.isNull()
			; child = child.nextSibling())
	{
		if (!child.isElement()) {
			continue;
		}

		QDomElement const childElement = child.toElement();
		if (childElement.nodeName() != "powerMotor") {
			qDebug() << "Malformed <powerMotors> tag";
			throw "config.xml parsing failed";
		}

		PowerMotorMapping mapping;
		mapping.port = childElement.attribute("port");
		mapping.i2cCommandNumber = childElement.attribute("i2cCommandNumber").toInt(NULL, 0);
		mapping.invert = childElement.attribute("invert") == "true";

		mPowerMotorMappings.insert(mapping.port, mapping);
	}
}

void Configurer::loadAnalogSensors(QDomElement const &root)
{
	if (root.elementsByTagName("analogSensors").isEmpty()) {
		qDebug() << "config.xml does not have <analogSensors> tag";
		throw "config.xml parsing failed";
	}

	QDomElement const analogSensors = root.elementsByTagName("analogSensors").at(0).toElement();
	for (QDomNode child = analogSensors.firstChild()
			; !child.isNull()
			; child = child.nextSibling())
	{
		if (!child.isElement()) {
			continue;
		}

		QDomElement const childElement = child.toElement();
		if (childElement.nodeName() != "analogSensor") {
			qDebug() << "Malformed <analogSensors> tag";
			throw "config.xml parsing failed";
		}

		AnalogSensorMapping mapping;
		mapping.port = childElement.attribute("port");
		mapping.i2cCommandNumber = childElement.attribute("i2cCommandNumber").toInt(NULL, 0);

		mAnalogSensorMappings.insert(mapping.port, mapping);
	}
}

void Configurer::loadEncoders(QDomElement const &root)
{
	if (root.elementsByTagName("encoders").isEmpty()) {
		qDebug() << "config.xml does not have <encoders> tag";
		throw "config.xml parsing failed";
	}

	QDomElement const encoders = root.elementsByTagName("encoders").at(0).toElement();
	for (QDomNode child = encoders.firstChild()
			; !child.isNull()
			; child = child.nextSibling())
	{
		if (!child.isElement()) {
			continue;
		}

		QDomElement const childElement = child.toElement();
		if (childElement.nodeName() != "encoder") {
			qDebug() << "Malformed <encoders> tag";
			throw "config.xml parsing failed";
		}

		EncoderMapping mapping;
		mapping.port = childElement.attribute("port");
		mapping.i2cCommandNumber = childElement.attribute("i2cCommandNumber").toInt(NULL, 0);

		mEncoderMappings.insert(mapping.port, mapping);
	}
}

void Configurer::loadDigitalSensors(QDomElement const &root)
{
	if (root.elementsByTagName("digitalSensors").isEmpty()) {
		qDebug() << "config.xml does not have <digitalSensors> tag";
		throw "config.xml parsing failed";
	}

	QDomElement const digitalSensors = root.elementsByTagName("digitalSensors").at(0).toElement();
	for (QDomNode child = digitalSensors.firstChild()
			; !child.isNull()
			; child = child.nextSibling())
	{
		if (!child.isElement()) {
			continue;
		}

		QDomElement const childElement = child.toElement();
		if (childElement.nodeName() != "digitalSensor") {
			continue;
		}

		DigitalSensorMapping mapping;
		mapping.port = childElement.attribute("port");
		mapping.deviceFile = childElement.attribute("deviceFile");
		mapping.defaultType = childElement.attribute("defaultType");

		mDigitalSensorMappings.insert(mapping.port, mapping);
	}
}

void Configurer::loadServoMotorTypes(QDomElement const &root)
{
	if (root.elementsByTagName("servoMotorTypes").isEmpty()) {
		qDebug() << "config.xml does not have <servoMotorTypes> tag";
		throw "config.xml parsing failed";
	}

	QDomElement const servoMotorTypes = root.elementsByTagName("servoMotorTypes").at(0).toElement();
	for (QDomNode child = servoMotorTypes.firstChild()
			; !child.isNull()
			; child = child.nextSibling())
	{
		if (!child.isElement()) {
			continue;
		}

		QDomElement const childElement = child.toElement();
		QString const typeName = childElement.nodeName();

		ServoMotorType servoMotorType;
		servoMotorType.min = childElement.attribute("min").toInt();
		servoMotorType.max = childElement.attribute("max").toInt();
		servoMotorType.zero = childElement.attribute("zero").toInt();
		servoMotorType.stop = childElement.attribute("stop").toInt();

		mServoMotorTypes.insert(typeName, servoMotorType);
	}
}

void Configurer::loadDigitalSensorTypes(QDomElement const &root)
{
	if (root.elementsByTagName("digitalSensorTypes").isEmpty()) {
		qDebug() << "config.xml does not have <digitalSensorTypes> tag";
		throw "config.xml parsing failed";
	}

	QDomElement const digitalSensorTypes = root.elementsByTagName("digitalSensorTypes").at(0).toElement();
	for (QDomNode child = digitalSensorTypes.firstChild()
			; !child.isNull()
			; child = child.nextSibling())
	{
		if (!child.isElement()) {
			continue;
		}

		QDomElement const childElement = child.toElement();
		QString const typeName = childElement.nodeName();

		DigitalSensorType digitalSensorType;
		digitalSensorType.min = childElement.attribute("min").toInt();
		digitalSensorType.max = childElement.attribute("max").toInt();

		mDigitalSensorTypes.insert(typeName, digitalSensorType);
	}
}

void Configurer::loadSound(QDomElement const &root)
{
	if (root.elementsByTagName("playWavFile").isEmpty()) {
		qDebug() << "config.xml does not have <playWavFile> tag";
		throw "config.xml parsing failed";
	}

	mPlayWavFileCommand = root.elementsByTagName("playWavFile").at(0).toElement().attribute("command");

	if (root.elementsByTagName("playMp3File").isEmpty()) {
		qDebug() << "config.xml does not have <playMp3File> tag";
		throw "config.xml parsing failed";
	}

	mPlayMp3FileCommand = root.elementsByTagName("playMp3File").at(0).toElement().attribute("command");

}

Configurer::OnBoardSensor Configurer::loadSensor3d(QDomElement const &root, QString const &tagName)
{
	if (root.elementsByTagName(tagName).isEmpty()) {
		qDebug() << "config.xml does not have <" << tagName << "> tag";
		throw "config.xml parsing failed";
	}

	if (root.elementsByTagName(tagName).count() > 1) {
		qDebug() << "config.xml has too many <" << tagName << "> tags, there shall be exactly one";
		throw "config.xml parsing failed";
	}

	OnBoardSensor result = {0, 0, ""};

	QDomElement const sensor = root.elementsByTagName(tagName).at(0).toElement();

	result.min = sensor.attribute("min").toInt();
	result.max = sensor.attribute("max").toInt();
	result.deviceFile = sensor.attribute("deviceFile");

	return result;
}

void Configurer::loadI2c(QDomElement const &root)
{
	mI2cPath = root.elementsByTagName("i2c").at(0).toElement().attribute("path");
	mI2cDeviceId = root.elementsByTagName("i2c").at(0).toElement().attribute("deviceId").toInt(NULL, 0);
}

void Configurer::loadLed(QDomElement const &root)
{
	QDomElement led = root.elementsByTagName("led").at(0).toElement();
	mLedRedDeviceFile = led.attribute("red");
	mLedGreenDeviceFile = led.attribute("green");
	mLedOn = led.attribute("on").toInt(NULL, 0);
	mLedOff = led.attribute("off").toInt(NULL, 0);
}

void Configurer::loadKeys(QDomElement const &root)
{
	QDomElement keys = root.elementsByTagName("keys").at(0).toElement();
	mKeysDeviceFile = keys.attribute("deviceFile");
}

void Configurer::loadGamepadPort(QDomElement const &root)
{
	QDomElement gamepad = root.elementsByTagName("gamepad").at(0).toElement();
	mGamepadPort = gamepad.attribute("port").toInt(NULL, 0);
}
