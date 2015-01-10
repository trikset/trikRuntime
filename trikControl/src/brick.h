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

#pragma once

#include <QtCore/QHash>
#include <QtCore/QScopedPointer>

#include "brickInterface.h"

#include <trikKernel/lazyMainWidget.h>

namespace trikControl {

class Configurer;
class I2cCommunicator;
class PowerMotor;
class ServoMotor;
class AnalogSensor;
class DigitalSensor;
class RangeSensor;
class Battery;
class VectorSensor;
class Display;
class LineSensor;
class ColorSensor;
class ObjectSensor;
class Keys;
class Gamepad;
class PwmCapture;
class Led;
class Encoder;
class Mailbox;

/// Class representing TRIK controller board and devices installed on it, also provides access
/// to peripherals like motors and sensors.
class Brick : public BrickInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param guiThread - thread in which an application has started. Can be obtaned in main() by code like
	///        QApplication app; app.thread();
	/// @param configFilePath - path to config.xml
	/// @param startDirPath - path to the directory from which the application was executed (it is expected to be
	///        ending with "/").
	Brick(QThread &guiThread, QString const &configFilePath, QString const &startDirPath);

	~Brick() override;

	void reset() override;

	trikKernel::LazyMainWidget &graphicsWidget() override;

public slots:
	void playSound(QString const &soundFileName) override;

	void say(QString const &text) override;

	void stop() override;

	MotorInterface *motor(QString const &port) override;

	PwmCaptureInterface *pwmCapture(QString const &port) override;

	SensorInterface *sensor(QString const &port) override;

	QStringList motorPorts(MotorInterface::Type type) const override;

	QStringList pwmCapturePorts() const override;

	QStringList sensorPorts(SensorInterface::Type type) const override;

	QStringList encoderPorts() const override;

	VectorSensorInterface *accelerometer() override;

	VectorSensorInterface *gyroscope() override;

	LineSensorInterface *lineSensor() override;

	ColorSensorInterface *colorSensor() override;

	ObjectSensorInterface *objectSensor() override;

	EncoderInterface *encoder(QString const &port) override;

	BatteryInterface *battery() override;

	KeysInterface *keys() override;

	GamepadInterface *gamepad() override;

	DisplayInterface *display() override;

	LedInterface *led() override;

	MailboxInterface *mailbox() override;

private:
	VectorSensor *mAccelerometer = nullptr;  // has ownership.
	VectorSensor *mGyroscope = nullptr;  // has ownership.
	LineSensor *mLineSensor = nullptr;  // Has ownership.
	ColorSensor *mColorSensor = nullptr;  // Has ownership.
	ObjectSensor *mObjectSensor = nullptr;  // Has ownership.
	Battery *mBattery = nullptr;  // Has ownership.
	Keys *mKeys = nullptr;  // Has ownership.
	Gamepad *mGamepad = nullptr;  // Has ownership.

	QHash<QString, ServoMotor *> mServoMotors;  // Has ownership.
	QHash<QString, PwmCapture *> mPwmCaptures;  // Has ownership.
	QHash<QString, PowerMotor *> mPowerMotors;  // Has ownership.
	QHash<QString, AnalogSensor *> mAnalogSensors;  // Has ownership.
	QHash<QString, Encoder *> mEncoders;  // Has ownership.
	QHash<QString, DigitalSensor *> mDigitalSensors;  // Has ownership.
	QHash<QString, RangeSensor *> mRangeSensors;  // Has ownership.

	Configurer const * const mConfigurer;  // Has ownership.
	I2cCommunicator *mI2cCommunicator = nullptr;  // Has ownership.
	QScopedPointer<Display> mDisplay;
	Led *mLed = nullptr;  // Has ownership.
	QScopedPointer<Mailbox> mMailbox;
};

}
