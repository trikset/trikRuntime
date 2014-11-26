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

#include "QsLog.h"

using namespace trikControl;

QString const errorMessage = "config.xml parsing failed";

Configurer::Configurer(QString const &configFilePath)
	: mI2cDeviceId(0)
	, mLedOn(0)
	, mLedOff(0)
	, mGamepadPort(0)
{
	QDomDocument config("config");

	QFile file(configFilePath + "config.xml");
	if (!file.open(QIODevice::ReadOnly)) {
		QString const message = "Failed to open config.xml for reading";
		QLOG_FATAL() << message;
		qDebug() << message;
		throw message;
	} if (!config.setContent(&file)) {
		file.close();
		QLOG_FATAL() << errorMessage;
		qDebug() << errorMessage;
		throw errorMessage;
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
	loadAnalogSensorTypes(root);
	loadDigitalSensorTypes(root);
	loadEncoderTypes(root);
	loadSound(root);

	mAccelerometer = loadSensor3d(root, "accelerometer");
	mGyroscope = loadSensor3d(root, "gyroscope");

	loadI2c(root);
	loadLed(root);
	loadKeys(root);
	loadGamepadPort(root);
	mLineSensor = loadVirtualSensor(root, "lineSensor");
	mObjectSensor = loadVirtualSensor(root, "objectSensor");
	mMxNColorSensor = loadVirtualSensor(root, "colorSensor");
	loadMailbox(root);
}

QString Configurer::initScript() const
{
	return mInitScript;
}

QStringList Configurer::servoMotorTypes() const
{
	return mServoMotorTypes.keys();
}

QStringList Configurer::analogSensorTypes() const
{
	return mAnalogSensorTypes.keys();
}

int Configurer::analogSensorTypeRawValue1(const QString &analogSensorType) const
{
	return mAnalogSensorTypes[analogSensorType].rawValue1;
}

int Configurer::analogSensorTypeRawValue2(const QString &analogSensorType) const
{
	return mAnalogSensorTypes[analogSensorType].rawValue2;
}

int Configurer::analogSensorTypeNormalizedValue1(const QString &analogSensorType) const
{
	return mAnalogSensorTypes[analogSensorType].normalizedValue1;
}

int Configurer::analogSensorTypeNormalizedValue2(const QString &analogSensorType) const
{
	return mAnalogSensorTypes[analogSensorType].normalizedValue2;
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

bool Configurer::isServoMotorTypeContiniousRotation(QString const &servoMotorType) const
{
	return mServoMotorTypes[servoMotorType].type == continiousRotation;
}

int Configurer::digitalSensorTypeMin(QString const &digitalSensorType) const
{
	return mDigitalSensorTypes[digitalSensorType].min;
}

int Configurer::digitalSensorTypeMax(QString const &digitalSensorType) const
{
	return mDigitalSensorTypes[digitalSensorType].max;
}

double Configurer::encoderTypeRawToDegrees(QString const &encoderType) const
{
	return mEncoderTypes[encoderType].rawToDegrees;
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

QString Configurer::analogSensorDefaultType(const QString &port) const
{
	return mAnalogSensorMappings[port].defaultType;
}

QStringList Configurer::encoderPorts() const
{
	return mEncoderMappings.keys();
}

int Configurer::encoderI2cCommandNumber(QString const &port) const
{
	return mEncoderMappings[port].i2cCommandNumber;
}

QString Configurer::encoderDefaultType(QString const &port) const
{
	return mEncoderMappings[port].defaultType;
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

bool Configurer::hasAccelerometer() const
{
	return mAccelerometer.enabled;
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

bool Configurer::hasGyroscope() const
{
	return mGyroscope.enabled;
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

bool Configurer::hasGamepad() const
{
	return mIsGamepadEnabled;
}

int Configurer::gamepadPort() const
{
	return mGamepadPort;
}

bool Configurer::hasLineSensor() const
{
	return mLineSensor.enabled;
}

QString Configurer::lineSensorScript() const
{
	return mLineSensor.script;
}

QString Configurer::lineSensorInFifo() const
{
	return mLineSensor.inFifo;
}

QString Configurer::lineSensorOutFifo() const
{
	return mLineSensor.outFifo;
}

double Configurer::lineSensorToleranceFactor() const
{
	return mLineSensor.toleranceFactor;
}

bool Configurer::hasObjectSensor() const
{
	return mObjectSensor.enabled;
}

QString Configurer::objectSensorScript() const
{
	return mObjectSensor.script;
}

QString Configurer::objectSensorInFifo() const
{
	return mObjectSensor.inFifo;
}

QString Configurer::objectSensorOutFifo() const
{
	return mObjectSensor.outFifo;
}

double Configurer::objectSensorToleranceFactor() const
{
	return mObjectSensor.toleranceFactor;
}

bool Configurer::hasColorSensor() const
{
	return mMxNColorSensor.enabled;
}

QString Configurer::colorSensorScript() const
{
	return mMxNColorSensor.script;
}

QString Configurer::colorSensorInFifo() const
{
	return mMxNColorSensor.inFifo;
}

QString Configurer::colorSensorOutFifo() const
{
	return mMxNColorSensor.outFifo;
}

int Configurer::colorSensorM() const
{
	return mColorSensorM;
}

int Configurer::colorSensorN() const
{
	return mColorSensorN;
}

bool Configurer::hasMailbox() const
{
	return mIsMailboxEnabled;
}

int Configurer::mailboxServerPort() const
{
	return mMailboxServerPort;
}

void Configurer::loadInit(QDomElement const &root)
{
	if (root.elementsByTagName("initScript").isEmpty()) {
		QLOG_FATAL() << "config.xml does not have <initScript> tag";
		qDebug() << "config.xml does not have <initScript> tag";
		throw errorMessage;
	}

	QDomElement const init = root.elementsByTagName("initScript").at(0).toElement();
	mInitScript = init.text();
}

void Configurer::loadServoMotors(QDomElement const &root)
{
	if (root.elementsByTagName("servoMotors").isEmpty()) {
		QLOG_FATAL() << "config.xml does not have <servoMotors> tag";
		qDebug() << "config.xml does not have <servoMotors> tag";
		throw errorMessage;
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
			QLOG_FATAL() << "Malformed <servoMotors> tag";
			qDebug() << "Malformed <servoMotors> tag";
			throw errorMessage;
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
		QLOG_FATAL() << "config.xml does not have <pwmCaptures> tag";
		qDebug() << "config.xml does not have <pwmCaptures> tag";
		throw errorMessage;
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
			QLOG_FATAL() << "Malformed <pwmCaptures> tag";
			qDebug() << "Malformed <pwmCaptures> tag";
			throw errorMessage;
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
		QLOG_FATAL() << "config.xml does not have <powerMotors> tag";
		qDebug() << "config.xml does not have <powerMotors> tag";
		throw errorMessage;
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
			QLOG_FATAL() << "Malformed <powerMotors> tag";
			qDebug() << "Malformed <powerMotors> tag";
			throw errorMessage;
		}

		PowerMotorMapping mapping;
		mapping.port = childElement.attribute("port");
		mapping.i2cCommandNumber = childElement.attribute("i2cCommandNumber").toInt(nullptr, 0);
		mapping.invert = childElement.attribute("invert") == "true";

		mPowerMotorMappings.insert(mapping.port, mapping);
	}
}

void Configurer::loadAnalogSensors(QDomElement const &root)
{
	if (root.elementsByTagName("analogSensors").isEmpty()) {
		QLOG_FATAL() << "config.xml does not have <analogSensors> tag";
		qDebug() << "config.xml does not have <analogSensors> tag";
		throw errorMessage;
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
			QLOG_FATAL() << "Malformed <analogSensors> tag";
			qDebug() << "Malformed <analogSensors> tag";
			throw errorMessage;
		}

		AnalogSensorMapping mapping;
		mapping.port = childElement.attribute("port");
		mapping.i2cCommandNumber = childElement.attribute("i2cCommandNumber").toInt(nullptr, 0);
		mapping.defaultType = childElement.attribute("defaultType");

		mAnalogSensorMappings.insert(mapping.port, mapping);
	}
}

void Configurer::loadEncoders(QDomElement const &root)
{
	if (root.elementsByTagName("encoders").isEmpty()) {
		QLOG_FATAL() << "config.xml does not have <encoders> tag";
		qDebug() << "config.xml does not have <encoders> tag";
		throw errorMessage;
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
			QLOG_FATAL() << "Malformed <encoders> tag";
			qDebug() << "Malformed <encoders> tag";
			throw errorMessage;
		}

		EncoderMapping mapping;
		mapping.port = childElement.attribute("port");
		mapping.i2cCommandNumber = childElement.attribute("i2cCommandNumber").toInt(nullptr, 0);
		mapping.defaultType = childElement.attribute("defaultType");

		mEncoderMappings.insert(mapping.port, mapping);
	}
}

void Configurer::loadDigitalSensors(QDomElement const &root)
{
	if (root.elementsByTagName("digitalSensors").isEmpty()) {
		QLOG_FATAL() << "config.xml does not have <digitalSensors> tag";
		qDebug() << "config.xml does not have <digitalSensors> tag";
		throw errorMessage;
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
		QLOG_FATAL() << "config.xml does not have <servoMotorTypes> tag";
		qDebug() << "config.xml does not have <servoMotorTypes> tag";
		throw errorMessage;
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
		servoMotorType.type = childElement.attribute("type") != "angular" ? continiousRotation : angular;

		mServoMotorTypes.insert(typeName, servoMotorType);
	}
}

void Configurer::loadAnalogSensorTypes(const QDomElement &root)
{
	if (root.elementsByTagName("analogSensorTypes").isEmpty()) {
		QLOG_FATAL() << "config.xml does not have <analogSensorTypes> tag";
		qDebug() << "config.xml does not have <analogSensorTypes> tag";
		throw errorMessage;
	}

	QDomElement const analogSensorTypes = root.elementsByTagName("analogSensorTypes").at(0).toElement();
	for (QDomNode child = analogSensorTypes.firstChild()
			; !child.isNull()
			; child = child.nextSibling())
	{
		if (!child.isElement()) {
			continue;
		}

		QDomElement const childElement = child.toElement();
		QString const typeName = childElement.nodeName();

		AnalogSensorType analogSensorType;
		analogSensorType.rawValue1 = childElement.attribute("rawValue1").toInt();
		analogSensorType.rawValue2 = childElement.attribute("rawValue2").toInt();
		analogSensorType.normalizedValue1 = childElement.attribute("normalizedValue1").toInt();
		analogSensorType.normalizedValue2 = childElement.attribute("normalizedValue2").toInt();

		mAnalogSensorTypes.insert(typeName, analogSensorType);
	}
}

void Configurer::loadDigitalSensorTypes(QDomElement const &root)
{
	if (root.elementsByTagName("digitalSensorTypes").isEmpty()) {
		QLOG_FATAL() << "config.xml does not have <digitalSensorTypes> tag";
		qDebug() << "config.xml does not have <digitalSensorTypes> tag";
		throw errorMessage;
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

void Configurer::loadEncoderTypes(const QDomElement &root)
{
	if (root.elementsByTagName("encoderTypes").isEmpty()) {
		QLOG_FATAL() << "config.xml does not have <encoderTypes> tag";
		qDebug() << "config.xml does not have <encoderTypes> tag";
		throw errorMessage;
	}

	QDomElement const encoderTypes = root.elementsByTagName("encoderTypes").at(0).toElement();
	for (QDomNode child = encoderTypes.firstChild()
			; !child.isNull()
			; child = child.nextSibling())
	{
		if (!child.isElement()) {
			continue;
		}

		QDomElement const childElement = child.toElement();
		QString const typeName = childElement.nodeName();

		EncoderType encoderType;
		encoderType.rawToDegrees = childElement.attribute("rawToDegrees").toDouble();
		mEncoderTypes.insert(typeName, encoderType);
	}
}

void Configurer::loadSound(QDomElement const &root)
{
	if (root.elementsByTagName("playWavFile").isEmpty()) {
		QLOG_FATAL() << "config.xml does not have <playWavFile> tag";
		qDebug() << "config.xml does not have <playWavFile> tag";
		throw errorMessage;
	}

	mPlayWavFileCommand = root.elementsByTagName("playWavFile").at(0).toElement().attribute("command");

	if (root.elementsByTagName("playMp3File").isEmpty()) {
		QLOG_FATAL() << "config.xml does not have <playMp3File> tag";
		qDebug() << "config.xml does not have <playMp3File> tag";
		throw errorMessage;
	}

	mPlayMp3FileCommand = root.elementsByTagName("playMp3File").at(0).toElement().attribute("command");

}

Configurer::OnBoardSensor Configurer::loadSensor3d(QDomElement const &root, QString const &tagName)
{
	OnBoardSensor result;

	if (isEnabled(root, tagName)) {

		if (root.elementsByTagName(tagName).count() > 1) {
			QLOG_FATAL() << "config.xml has too many <" << tagName << "> tags, there shall be exactly one";
			qDebug() << "config.xml has too many <" << tagName << "> tags, there shall be exactly one";
			throw errorMessage;
		}

		QDomElement const sensor = root.elementsByTagName(tagName).at(0).toElement();

		result.min = sensor.attribute("min").toInt();
		result.max = sensor.attribute("max").toInt();
		result.deviceFile = QString(sensor.attribute("deviceFile"));
		result.enabled = true;
	}

	return result;
}

void Configurer::loadI2c(QDomElement const &root)
{
	mI2cPath = root.elementsByTagName("i2c").at(0).toElement().attribute("path");
	mI2cDeviceId = root.elementsByTagName("i2c").at(0).toElement().attribute("deviceId").toInt(nullptr, 0);
}

void Configurer::loadLed(QDomElement const &root)
{
	QDomElement led = root.elementsByTagName("led").at(0).toElement();
	mLedRedDeviceFile = led.attribute("red");
	mLedGreenDeviceFile = led.attribute("green");
	mLedOn = led.attribute("on").toInt(nullptr, 0);
	mLedOff = led.attribute("off").toInt(nullptr, 0);
}

void Configurer::loadKeys(QDomElement const &root)
{
	QDomElement keys = root.elementsByTagName("keys").at(0).toElement();
	mKeysDeviceFile = keys.attribute("deviceFile");
}

void Configurer::loadGamepadPort(QDomElement const &root)
{
	if (isEnabled(root, "gamepad")) {
		QDomElement gamepad = root.elementsByTagName("gamepad").at(0).toElement();
		mGamepadPort = gamepad.attribute("port").toInt(nullptr, 0);
		mIsGamepadEnabled = true;
	}
}

Configurer::VirtualSensor Configurer::loadVirtualSensor(QDomElement const &root, QString const &tagName)
{
	VirtualSensor result;
	if (isEnabled(root, tagName)) {
		QDomElement sensorElement = root.elementsByTagName(tagName).at(0).toElement();
		result.script = sensorElement.attribute("script");
		result.inFifo = sensorElement.attribute("inputFile");
		result.outFifo = sensorElement.attribute("outputFile");
		result.toleranceFactor = sensorElement.attribute("toleranceFactor", "1.0").toDouble();
		result.enabled = true;

		if (tagName == "colorSensor") {
			mColorSensorM = sensorElement.attribute("m").toInt();
			mColorSensorN = sensorElement.attribute("n").toInt();
		}
	}

	return result;
}

void Configurer::loadMailbox(QDomElement const &root)
{
	if (isEnabled(root, "mailbox")) {
		QDomElement mailboxElement = root.elementsByTagName("mailbox").at(0).toElement();
		mMailboxServerPort = mailboxElement.attribute("port").toInt();
		mIsMailboxEnabled = true;
	}
}

bool Configurer::isEnabled(QDomElement const &root, QString const &tagName)
{
	return root.elementsByTagName(tagName).size() > 0
			&& root.elementsByTagName(tagName).at(0).toElement().attribute("disabled") != "true";
}
