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

#pragma once

#include <QtCore/QString>
#include <QtCore/QList>
#include <QtCore/QStringList>
#include <QtCore/QHash>

namespace trikControl {

class Configurer {
public:
	Configurer();

	QString initScript() const;

	QStringList motorTypes() const;

	QStringList sensorTypes() const;

	int motorTypeMin(QString const &motorType) const;

	int motorTypeMax(QString const &motorType) const;

	int motorTypeZero(QString const &motorType) const;

	int motorTypeStop(QString const &motorType) const;

	int sensorTypeMin(QString const &sensorType) const;

	int sensorTypeMax(QString const &sensorType) const;

	QStringList servoMotorPorts() const;

	QString servoMotorDeviceFile(QString const &port) const;

	QString servoMotorDefaultType(QString const &port) const;

	bool servoMotorInvert(QString const &port) const;

	QStringList powerMotorPorts() const;

	QString powerMotorCommand(QString const &port) const;

	QString powerMotorStop(QString const &port) const;

	bool powerMotorInvert(QString const &port) const;

	QStringList sensorPorts() const;

	QString sensorDeviceFile(QString const &port) const;

	QString sensorDefaultType(QString const &port) const;

	QString playSoundCommand() const;

private:
	struct MotorType {
		int min;
		int max;
		int zero;
		int stop;
	};

	struct SensorType {
		int min;
		int max;
	};

	struct ServoMotorMapping {
		QString port;
		QString deviceFile;
		QString defaultType;
		bool invert;
	};

	struct PowerMotorMapping {
		QString port;
		QString command;
		QString stopCommand;
		bool invert;
	};

	struct SensorMapping {
		QString port;
		QString deviceFile;
		QString defaultType;
	};

	QHash<QString, MotorType> mMotorTypes;
	QHash<QString, SensorType> mSensorTypes;
	QHash<QString, ServoMotorMapping> mServoMotorMappings;
	QHash<QString, PowerMotorMapping> mPowerMotorMappings;
	QHash<QString, SensorMapping> mSensorMappings;

	QString mInitScript;
	QString mPlaySoundCommand;
};

}
