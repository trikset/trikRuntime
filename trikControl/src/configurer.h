/* Copyright 2014 CyberTech Labs Ltd.
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
	/// Constructor
	/// @param configFilePath - path to config.xml.
	Configurer(QString const &configFilePath);

	/// Returns sh script that shall be run on program startup.
	QString initScript() const;

	/// Returns a list of motor type names from config.
	QStringList servoMotorTypes() const;

	/// @todo: comment this.
	QStringList analogSensorTypes() const;

	/// @todo: comment this.
	int analogSensorTypeRawValue1(QString const &analogSensorType) const;

	/// @todo: comment this.
	int analogSensorTypeRawValue2(QString const &analogSensorType) const;

	/// @todo: comment this.
	int analogSensorTypeNormalizedValue1(QString const &analogSensorType) const;

	/// @todo: comment this.
	int analogSensorTypeNormalizedValue2(QString const &analogSensorType) const;

	/// Returns a list of sensor type names from config.
	QStringList digitalSensorTypes() const;

	/// Returns value of duty_ns corresponding to full reverse of a servo motor for given servo motor type.
	int servoMotorTypeMin(QString const &servoMotorType) const;

	/// Returns value of duty_ns corresponding to full forward of a servo motor for given servo motor type.
	int servoMotorTypeMax(QString const &servoMotorType) const;

	/// Returns value of duty_ns corresponding to breaking stop of a servo motor for given servo motor type.
	int servoMotorTypeZero(QString const &servoMotorType) const;

	/// Returns value of duty_ns corresponding to non-breaking stop of a servo motor for given servo motor type.
	int servoMotorTypeStop(QString const &servoMotorType) const;

	/// Returns true, if it is continious rotation servo, false for angular servos.
	bool isServoMotorTypeContiniousRotation(QString const &servoMotorType) const;

	/// Returns minimal physical reading value of a digital sensor (corresponds to 0 in client program).
	int digitalSensorTypeMin(QString const &digitalSensorType) const;

	/// @todo: comment this.
	int digitalSensorTypeMax(QString const &digitalSensorType) const;

	/// @todo: comment this.
	double encoderTypeRawToDegrees(QString const &encoderType) const;

	/// @todo: comment this.
	QStringList servoMotorPorts() const;

	/// @todo: comment this.
	QString servoMotorDeviceFile(QString const &port) const;

	/// @todo: comment this.
	QString servoMotorPeriodFile(QString const &port) const;

	/// @todo: comment this.
	int servoMotorPeriod(QString const &port) const;

	/// @todo: comment this.
	QString servoMotorDefaultType(QString const &port) const;

	/// @todo: comment this.
	bool servoMotorInvert(QString const &port) const;

	/// @todo: comment this.
	QStringList pwmCapturePorts() const;

	/// @todo: comment this.
	QString pwmCaptureFrequencyFile(QString const &port) const;

	/// @todo: comment this.
	QString pwmCaptureDutyFile(QString const &port) const;

	/// @todo: comment this.
	QStringList powerMotorPorts() const;

	/// @todo: comment this.
	int powerMotorI2cCommandNumber(QString const &port) const;

	/// @todo: comment this.
	bool powerMotorInvert(QString const &port) const;

	/// @todo: comment this.
	QStringList analogSensorPorts() const;

	/// @todo: comment this.
	int analogSensorI2cCommandNumber(QString const &port) const;

	/// @todo: comment this.
	QString analogSensorDefaultType(QString const &port) const;

	/// @todo: comment this.
	QStringList encoderPorts() const;

	/// @todo: comment this.
	int encoderI2cCommandNumber(QString const &port) const;

	/// @todo: comment this.
	QString encoderDefaultType(QString const &port) const;

	/// @todo: comment this.
	QStringList digitalSensorPorts() const;

	/// @todo: comment this.
	QString digitalSensorDeviceFile(QString const &port) const;

	/// @todo: comment this.
	QString digitalSensorDefaultType(QString const &port) const;

	/// @todo: comment this.
	QStringList rangeSensorPorts() const;

	/// @todo: comment this.
	QString rangeSensorEventFile(QString const &port) const;

	/// @todo: comment this.
	QString playWavFileCommand() const;

	/// @todo: comment this.
	QString playMp3FileCommand() const;

	/// @todo: comment this.
	bool hasAccelerometer() const;

	/// @todo: comment this.
	QString accelerometerDeviceFile() const;

	/// @todo: comment this.
	bool hasGyroscope() const;

	/// @todo: comment this.
	QString gyroscopeDeviceFile() const;

	/// @todo: comment this.
	QString i2cPath() const;

	/// @todo: comment this.
	int i2cDeviceId() const;

	/// @todo: comment this.
	QString ledRedDeviceFile() const;

	/// @todo: comment this.
	QString ledGreenDeviceFile() const;

	/// @todo: comment this.
	int ledOn() const;

	/// @todo: comment this.
	int ledOff() const;

	/// @todo: comment this.
	QString keysDeviceFile() const;

	/// @todo: comment this.
	bool hasGamepad() const;

	/// @todo: comment this.
	int gamepadPort() const;

	/// @todo: comment this.
	bool hasLineSensor() const;

	/// @todo: comment this.
	QString lineSensorScript() const;

	/// @todo: comment this.
	QString lineSensorInFifo() const;

	/// @todo: comment this.
	QString lineSensorOutFifo() const;

	/// @todo: comment this.
	double lineSensorToleranceFactor() const;

	/// @todo: comment this.
	bool hasObjectSensor() const;

	/// @todo: comment this.
	QString objectSensorScript() const;

	/// @todo: comment this.
	QString objectSensorInFifo() const;

	/// @todo: comment this.
	QString objectSensorOutFifo() const;

	/// @todo: comment this.
	double objectSensorToleranceFactor() const;

	/// @todo: comment this.
	bool hasColorSensor() const;

	/// @todo: comment this.
	QString colorSensorScript() const;

	/// @todo: comment this.
	QString colorSensorInFifo() const;

	/// @todo: comment this.
	QString colorSensorOutFifo() const;

	/// @todo: comment this.
	int colorSensorM() const;

	/// @todo: comment this.
	int colorSensorN() const;

	/// @todo: comment this.
	bool hasMailbox() const;

	/// @todo: comment this.
	int mailboxServerPort() const;

private:
	enum ServoType {
		angular
		, continiousRotation
	};

	struct ServoMotorType {
		int min;
		int max;
		int zero;
		int stop;
		ServoType type;
	};

	struct AnalogSensorType {
		int rawValue1;
		int rawValue2;
		int normalizedValue1;
		int normalizedValue2;
	};

	struct DigitalSensorType {
		int min;
		int max;
	};

	struct EncoderType {
		double rawToDegrees;
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
		QString defaultType;
	};

	struct EncoderMapping {
		QString port;
		int i2cCommandNumber;
		QString defaultType;
	};

	struct DigitalSensorMapping {
		QString port;
		QString deviceFile;
		QString defaultType;
	};

	struct RangeSensorMapping {
		QString port;
		QString eventFile;
	};

	struct OnBoardSensor {
		QString deviceFile;
		bool enabled = false;
	};

	struct VirtualSensor {
		QString script;
		QString inFifo;
		QString outFifo;
		double toleranceFactor = 1.0;
		bool enabled = false;
	};

	void loadInit(QDomElement const &root);
	void loadServoMotors(QDomElement const &root);
	void loadPwmCaptures(QDomElement const &root);
	void loadPowerMotors(QDomElement const &root);
	void loadAnalogSensors(QDomElement const &root);
	void loadEncoders(QDomElement const &root);
	void loadDigitalSensors(QDomElement const &root);
	void loadRangeSensors(QDomElement const &root);
	void loadServoMotorTypes(QDomElement const &root);
	void loadAnalogSensorTypes(QDomElement const &root);
	void loadDigitalSensorTypes(QDomElement const &root);
	void loadEncoderTypes(QDomElement const &root);
	void loadSound(QDomElement const &root);
	static OnBoardSensor loadVectorSensor(QDomElement const &root, QString const &tagName);
	void loadI2c(QDomElement const &root);
	void loadLed(QDomElement const &root);
	void loadKeys(QDomElement const &root);
	void loadGamepadPort(QDomElement const &root);
	VirtualSensor loadVirtualSensor(QDomElement const &root, QString const &tagName);
	void loadMailbox(QDomElement const &root);

	static bool isEnabled(QDomElement const &root, QString const &tagName);

	QHash<QString, ServoMotorType> mServoMotorTypes;
	QHash<QString, AnalogSensorType> mAnalogSensorTypes;
	QHash<QString, DigitalSensorType> mDigitalSensorTypes;
	QHash<QString, EncoderType> mEncoderTypes;
	QHash<QString, ServoMotorMapping> mServoMotorMappings;
	QHash<QString, PwmCaptureMapping> mPwmCaptureMappings;
	QHash<QString, PowerMotorMapping> mPowerMotorMappings;
	QHash<QString, AnalogSensorMapping> mAnalogSensorMappings;
	QHash<QString, EncoderMapping> mEncoderMappings;
	QHash<QString, DigitalSensorMapping> mDigitalSensorMappings;
	QHash<QString, RangeSensorMapping> mRangeSensorMappings;

	OnBoardSensor mAccelerometer;
	OnBoardSensor mGyroscope;

	QString mInitScript;
	QString mPlayWavFileCommand;
	QString mPlayMp3FileCommand;
	QString mI2cPath;
	int mI2cDeviceId = 0;

	QString mLedRedDeviceFile;
	QString mLedGreenDeviceFile;
	QString mKeysDeviceFile;
	int mLedOn = 0;
	int mLedOff = 0;

	int mGamepadPort = 0;
	bool mIsGamepadEnabled = false;

	VirtualSensor mLineSensor;
	VirtualSensor mObjectSensor;
	VirtualSensor mMxNColorSensor;
	int mColorSensorM = 0;
	int mColorSensorN = 0;

	int mMailboxServerPort = 0;
	bool mIsMailboxEnabled = false;
};

}
