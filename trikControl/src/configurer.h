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

class QDomElement;

namespace trikControl {

/// Parses config file and holds information about various configuration parameters.
class Configurer {
public:
	Configurer();

	/// Returns sh script that shall be run on program startup.
	QString initScript() const;

	/// Returns a list of motor type names from config.
	QStringList motorTypes() const;

	/// Returns a list of sensor type names from config.
	QStringList sensorTypes() const;

	/// Returns value of duty_ns corresponding to full reverse of a motor for given motor type.
	int motorTypeMin(QString const &motorType) const;

	/// Returns value of duty_ns corresponding to full forward of a motor for given motor type.
	int motorTypeMax(QString const &motorType) const;

	/// Returns value of duty_ns corresponding to breaking stop of a motor for given motor type.
	int motorTypeZero(QString const &motorType) const;

	/// Returns value of duty_ns corresponding to non-breaking stop of a motor for given motor type.
	int motorTypeStop(QString const &motorType) const;

	/// Returns minimal physical reading value of a sensor (corresponds to 0 in client program).
	int sensorTypeMin(QString const &sensorType) const;

	int sensorTypeMax(QString const &sensorType) const;

	QStringList servoMotorPorts() const;

	QString servoMotorDeviceFile(QString const &port) const;

	QString servoMotorPeriodFile(QString const &port) const;

	int servoMotorPeriod(QString const &port) const;

	QString servoMotorDefaultType(QString const &port) const;

	bool servoMotorInvert(QString const &port) const;

	QStringList pwmCapturePorts() const;

	QString pwmCaptureFrequencyFile(QString const &port) const;

	QString pwmCaptureDutyFile(QString const &port) const;

	QStringList powerMotorPorts() const;

	int powerMotorI2cCommandNumber(QString const &port) const;

	bool powerMotorInvert(QString const &port) const;

	QStringList analogSensorPorts() const;

	int analogSensorI2cCommandNumber(QString const &port) const;

	QStringList encoderPorts() const;

	int encoderI2cCommandNumber(QString const &port) const;

	QStringList sensorPorts() const;

	QString sensorDeviceFile(QString const &port) const;

	QString sensorDefaultType(QString const &port) const;

	QString playSoundCommand() const;

	int accelerometerMin() const;

	int accelerometerMax() const;

	QString accelerometerDeviceFile() const;

	int gyroscopeMin() const;

	int gyroscopeMax() const;

	QString gyroscopeDeviceFile() const;

	QString i2cPath() const;

	int i2cDeviceId() const;

	QString ledRedDeviceFile() const;

	QString ledGreenDeviceFile() const;

	int ledOn() const;

	int ledOff() const;

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
		QString periodFile;
		int period;
		QString defaultType;
		bool invert;
	};

	struct PwmCaptureMapping {
		QString port;
		QString frequencyFile;
		QString dutyFile;
	};

	struct PowerMotorMapping {
		QString port;
		int i2cCommandNumber;
		bool invert;
	};

	struct AnalogSensorMapping {
		QString port;
		int i2cCommandNumber;
	};

	struct EncoderMapping {
		QString port;
		int i2cCommandNumber;
	};

	struct SensorMapping {
		QString port;
		QString deviceFile;
		QString defaultType;
	};

	struct OnBoardSensor {
		int min;
		int max;
		QString deviceFile;
	};

	void loadInit(QDomElement const &root);
	void loadServoMotors(QDomElement const &root);
	void loadPwmCaptures(QDomElement const &root);
	void loadPowerMotors(QDomElement const &root);
	void loadAnalogSensors(QDomElement const &root);
	void loadEncoders(QDomElement const &root);
	void loadSensors(QDomElement const &root);
	void loadMotorTypes(QDomElement const &root);
	void loadSensorTypes(QDomElement const &root);
	void loadSound(QDomElement const &root);
	OnBoardSensor loadSensor3d(QDomElement const &root, QString const &tagName);
	void loadI2c(QDomElement const &root);
	void loadLed(QDomElement const &root);

	QHash<QString, MotorType> mMotorTypes;
	QHash<QString, SensorType> mSensorTypes;
	QHash<QString, ServoMotorMapping> mServoMotorMappings;
	QHash<QString, PwmCaptureMapping> mPwmCaptureMappings;
	QHash<QString, PowerMotorMapping> mPowerMotorMappings;
	QHash<QString, AnalogSensorMapping> mAnalogSensorMappings;
	QHash<QString, EncoderMapping> mEncoderMappings;
	QHash<QString, SensorMapping> mSensorMappings;

	OnBoardSensor mAccelerometer;
	OnBoardSensor mGyroscope;

	QString mInitScript;
	QString mPlaySoundCommand;
	QString mI2cPath;
	int mI2cDeviceId;

	QString mLedRedDeviceFile;
	QString mLedGreenDeviceFile;
	int mLedOn;
	int mLedOff;
};

}
