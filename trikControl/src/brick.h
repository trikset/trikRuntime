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

#include <trikKernel/configurer.h>

#include "brickInterface.h"

namespace trikHal {
class HardwareAbstractionInterface;
}

namespace trikControl {

class AnalogSensor;
class Battery;
class ColorSensor;
class DigitalSensor;
class Display;
class Encoder;
class MspCommunicatorInterface;
class Keys;
class Led;
class LineSensor;
class ModuleLoader;
class ObjectSensor;
class SoundSensor;
class PowerMotor;
class PwmCapture;
class RangeSensor;
class ServoMotor;
class VectorSensor;
class Fifo;

/// Class representing TRIK controller board and devices installed on it, also provides access
/// to peripherals like motors and sensors.
class Brick : public BrickInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param systemConfig - file name (with path) of system config, absolute or relative to current directory.
	/// @param modelConfig - file name (with path) of model config, absolute or relative to current directory.
	/// @param mediaPath - path to the directory with media files (it is expected to be ending with "/").
	Brick(const QString &systemConfig, const QString &modelConfig, const QString &mediaPath);

	/// Secondary constructor, takes explicit hardware abstraction object.
	/// @param hardwareAbstraction - hardware abstraction layer implementation.
	/// @param systemConfig - file name (with path) of system config, absolute or relative to current directory.
	/// @param modelConfig - file name (with path) of model config, absolute or relative to current directory.
	/// @param mediaPath - path to the directory with media files (it is expected to be ending with "/").
	Brick(trikHal::HardwareAbstractionInterface &hardwareAbstraction, const QString &systemConfig
			, const QString &modelConfig, const QString &mediaPath);

	~Brick() override;

	void reset() override;

	DisplayWidgetInterface &graphicsWidget() override;

public slots:
	void configure(const QString &portName, const QString &deviceName) override;

	void playSound(const QString &soundFileName) override;

	void say(const QString &text) override;

	void stop() override;

	MotorInterface *motor(const QString &port) override;

	PwmCaptureInterface *pwmCapture(const QString &port) override;

	SensorInterface *sensor(const QString &port) override;

	QStringList motorPorts(MotorInterface::Type type) const override;

	QStringList pwmCapturePorts() const override;

	QStringList sensorPorts(SensorInterface::Type type) const override;

	QStringList encoderPorts() const override;

	VectorSensorInterface *accelerometer() override;

	VectorSensorInterface *gyroscope() override;

	LineSensorInterface *lineSensor(const QString &port) override;

	ColorSensorInterface *colorSensor(const QString &port) override;

	ObjectSensorInterface *objectSensor(const QString &port) override;

	SoundSensorInterface *soundSensor(const QString &port) override;

	EncoderInterface *encoder(const QString &port) override;

	BatteryInterface *battery() override;

	KeysInterface *keys() override;

	DisplayInterface *display() override;

	LedInterface *led() override;

	FifoInterface *fifo(const QString &port) override;

private:
	Brick(trikHal::HardwareAbstractionInterface * const hardwareAbstraction, const QString &systemConfig
			, const QString &modelConfig, const QString &mediaPath, bool ownsHardwareAbstraction);

	/// Deinitializes and properly shuts down device on a given port.
	void shutdownDevice(const QString &port);

	/// Creates and configures a device on a given port.
	void createDevice(const QString &port);

	QScopedPointer<MspCommunicatorInterface> mMspCommunicator;
	QScopedPointer<ModuleLoader> mModuleLoader;

	QScopedPointer<VectorSensor> mAccelerometer;
	QScopedPointer<VectorSensor> mGyroscope;
	QScopedPointer<Battery> mBattery;
	QScopedPointer<Keys> mKeys;
	QScopedPointer<Display> mDisplay;
	QScopedPointer<Led> mLed;

	QHash<QString, ServoMotor *> mServoMotors;  // Has ownership.
	QHash<QString, PwmCapture *> mPwmCaptures;  // Has ownership.
	QHash<QString, PowerMotor *> mPowerMotors;  // Has ownership.
	QHash<QString, AnalogSensor *> mAnalogSensors;  // Has ownership.
	QHash<QString, Encoder *> mEncoders;  // Has ownership.
	QHash<QString, DigitalSensor *> mDigitalSensors;  // Has ownership.
	QHash<QString, RangeSensor *> mRangeSensors;  // Has ownership.
	QHash<QString, LineSensor *> mLineSensors;  // Has ownership.
	QHash<QString, ColorSensor *> mColorSensors;  // Has ownership.
	QHash<QString, ObjectSensor *> mObjectSensors;  // Has ownership.
	QHash<QString, SoundSensor *> mSoundSensors;  // Has ownership.
	QHash<QString, Fifo *> mFifos;  // Has ownership.

	QString mPlayWavFileCommand;
	QString mPlayMp3FileCommand;

	/// Hardware absraction object that is used to provide communication with real robot hardware or to simulate it.
	/// Has or hasn't ownership depending on mOwnsHardwareAbstraction value.
	trikHal::HardwareAbstractionInterface *mHardwareAbstraction;

	/// True, if hardware abstraction object was created here, false if passed from outside.
	bool mOwnsHardwareAbstraction;

	trikKernel::Configurer mConfigurer;
};

}
