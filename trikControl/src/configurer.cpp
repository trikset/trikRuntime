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

Configurer::Configurer()
{
	QDomDocument config("config");

	QFile file("config.xml");
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

	if (root.elementsByTagName("init").isEmpty()) {
		qDebug() << "config.xml does not have <init> tag";
		throw "config.xml parsing failed";
	}

	QDomElement const init = root.elementsByTagName("initScript").at(0).toElement();
	mInitScript = init.text();

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
		if (childElement.nodeName() != "motor") {
			qDebug() << "Malformed <servoMotors> tag";
			throw "config.xml parsing failed";
		}

		ServoMotorMapping mapping;
		mapping.port = childElement.attribute("port");
		mapping.deviceFile = childElement.attribute("deviceFile");
		mapping.defaultType = childElement.attribute("defaultType");
		mapping.invert = childElement.attribute("invert") == "true";

		mServoMotorMappings.insert(mapping.port, mapping);
	}

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
		if (childElement.nodeName() != "motor") {
			qDebug() << "Malformed <powerMotors> tag";
			throw "config.xml parsing failed";
		}

		PowerMotorMapping mapping;
		mapping.port = childElement.attribute("port");
		mapping.command = childElement.attribute("command");
		mapping.stopCommand = childElement.attribute("stop");
		mapping.invert = childElement.attribute("invert") == "true";

		mPowerMotorMappings.insert(mapping.port, mapping);
	}

	if (root.elementsByTagName("sensors").isEmpty()) {
		qDebug() << "config.xml does not have <sensors> tag";
		throw "config.xml parsing failed";
	}

	QDomElement const sensors = root.elementsByTagName("sensors").at(0).toElement();
	for (QDomNode child = sensors.firstChild()
			; !child.isNull()
			; child = child.nextSibling())
	{
		if (!child.isElement()) {
			continue;
		}

		QDomElement const childElement = child.toElement();
		if (childElement.nodeName() != "sensor") {
			qDebug() << "Malformed <sensors> tag";
			throw "config.xml parsing failed";
		}

		SensorMapping mapping;
		mapping.port = childElement.attribute("port");
		mapping.deviceFile = childElement.attribute("deviceFile");
		mapping.defaultType = childElement.attribute("defaultType");

		mSensorMappings.insert(mapping.port, mapping);
	}

	if (root.elementsByTagName("motorTypes").isEmpty()) {
		qDebug() << "config.xml does not have <motorTypes> tag";
		throw "config.xml parsing failed";
	}

	QDomElement const motorTypes = root.elementsByTagName("motorTypes").at(0).toElement();
	for (QDomNode child = motorTypes.firstChild()
			; !child.isNull()
			; child = child.nextSibling())
	{
		if (!child.isElement()) {
			continue;
		}

		QDomElement const childElement = child.toElement();
		QString const typeName = childElement.nodeName();

		MotorType motorType;
		motorType.min = childElement.attribute("min").toInt();
		motorType.max = childElement.attribute("max").toInt();
		motorType.zero = childElement.attribute("zero").toInt();
		motorType.stop = childElement.attribute("stop").toInt();

		mMotorTypes.insert(typeName, motorType);
	}

	if (root.elementsByTagName("sensorTypes").isEmpty()) {
		qDebug() << "config.xml does not have <sensorTypes> tag";
		throw "config.xml parsing failed";
	}

	QDomElement const sensorTypes = root.elementsByTagName("sensorTypes").at(0).toElement();
	for (QDomNode child = sensorTypes.firstChild()
			; !child.isNull()
			; child = child.nextSibling())
	{
		if (!child.isElement()) {
			continue;
		}

		QDomElement const childElement = child.toElement();
		QString const typeName = childElement.nodeName();

		SensorType sensorType;
		sensorType.min = childElement.attribute("min").toInt();
		sensorType.max = childElement.attribute("max").toInt();

		mSensorTypes.insert(typeName, sensorType);
	}

	if (root.elementsByTagName("playSound").isEmpty()) {
		qDebug() << "config.xml does not have <playSound> tag";
		throw "config.xml parsing failed";
	}

	mPlaySoundCommand = root.elementsByTagName("playSound").at(0).toElement().attribute("command");
}

QString Configurer::initScript() const
{
	return mInitScript;
}

QStringList Configurer::motorTypes() const
{
	return mMotorTypes.keys();
}

QStringList Configurer::sensorTypes() const
{
	return mSensorTypes.keys();
}

int Configurer::motorTypeMin(QString const &motorType) const
{
	return mMotorTypes[motorType].min;
}

int Configurer::motorTypeMax(QString const &motorType) const
{
	return mMotorTypes[motorType].max;
}

int Configurer::motorTypeZero(QString const &motorType) const
{
	return mMotorTypes[motorType].zero;
}

int Configurer::motorTypeStop(QString const &motorType) const
{
	return mMotorTypes[motorType].stop;
}

int Configurer::sensorTypeMin(QString const &sensorType) const
{
	return mSensorTypes[sensorType].min;
}

int Configurer::sensorTypeMax(QString const &sensorType) const
{
	return mSensorTypes[sensorType].max;
}

QStringList Configurer::servoMotorPorts() const
{
	return mServoMotorMappings.keys();
}

QString Configurer::servoMotorDeviceFile(QString const &port) const
{
	return mServoMotorMappings[port].deviceFile;
}

QString Configurer::servoMotorDefaultType(QString const &port) const
{
	return mServoMotorMappings[port].defaultType;
}

bool Configurer::servoMotorInvert(QString const &port) const
{
	return mServoMotorMappings[port].invert;
}

QStringList Configurer::powerMotorPorts() const
{
	return mPowerMotorMappings.keys();
}

QString Configurer::powerMotorCommand(QString const &port) const
{
	return mPowerMotorMappings[port].command;
}

QString Configurer::powerMotorStop(QString const &port) const
{
	return mPowerMotorMappings[port].stopCommand;
}

bool Configurer::powerMotorInvert(QString const &port) const
{
	return mPowerMotorMappings[port].invert;
}

QStringList Configurer::sensorPorts() const
{
	return mSensorMappings.keys();
}

QString Configurer::sensorDeviceFile(QString const &port) const
{
	return mSensorMappings[port].deviceFile;
}

QString Configurer::sensorDefaultType(QString const &port) const
{
	return mSensorMappings[port].defaultType;
}

QString Configurer::playSoundCommand() const
{
	return mPlaySoundCommand;
}
