/* Copyright 2013 - 2016 Yurii Litvinov and CyberTech Labs Ltd.
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
#include <trikKernel/differentOwnerPointer.h>

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
class EventDevice;
class Fifo;
class Gamepad;
class GyroSensor;
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
class TonePlayer;
class VectorSensor;

/// Class representing TRIK controller board and devices installed on it, also provides access
/// to peripherals like motors and sensors.
/// Is NOT thread-safe.
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

	DisplayWidgetInterface *graphicsWidget() override;

	QString configVersion() const override;

public slots:
	void configure(const QString &portName, const QString &deviceName) override;

	void playSound(const QString &soundFileName) override;

	void playTone(int hzFreq, int msDuration) override;

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

	GyroSensorInterface *gyroscope() override;

	LineSensorInterface *lineSensor(const QString &port) override;

	ColorSensorInterface *colorSensor(const QString &port) override;

	ObjectSensorInterface *objectSensor(const QString &port) override;

	SoundSensorInterface *soundSensor(const QString &port) override;

	EncoderInterface *encoder(const QString &port) override;

	BatteryInterface *battery() override;

	KeysInterface *keys() override;

	DisplayInterface *display() override;

	LedInterface *led() override;

	GamepadInterface *gamepad() override;

	FifoInterface *fifo(const QString &port) override;

	EventDeviceInterface *eventDevice(const QString &deviceFile) override;

	void stopEventDevice(const QString &deviceFile) override;

private:
	Brick(const trikKernel::DifferentOwnerPointer<trikHal::HardwareAbstractionInterface> &hardwareAbstraction
			, const QString &systemConfig
			, const QString &modelConfig
			, const QString &mediaPath);

	/// Deinitializes and properly shuts down device on a given port.
	void shutdownDevice(const QString &port);

	/// Creates and configures a device on a given port.
	void createDevice(const QString &port);

	/// Hardware absraction object that is used to provide communication with real robot hardware or to simulate it.
	/// Has or hasn't ownership depending on whether it was created by Brick itself or passed from outside.
	trikKernel::DifferentOwnerPointer<trikHal::HardwareAbstractionInterface> mHardwareAbstraction;

	QScopedPointer<MspCommunicatorInterface> mMspCommunicator;
	QScopedPointer<ModuleLoader> mModuleLoader;

	QScopedPointer<VectorSensor> mAccelerometer;
	QScopedPointer<GyroSensor> mGyroscope;
	QScopedPointer<Battery> mBattery;
	QScopedPointer<Keys> mKeys;
	QScopedPointer<Display> mDisplay;
	QScopedPointer<Led> mLed;
	QScopedPointer<Gamepad> mGamepad;
	QScopedPointer<TonePlayer> mTonePlayer;

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
	QHash<QString, EventDeviceInterface *> mEventDevices;  // Has ownership.

	QString mPlayWavFileCommand;
	QString mPlayMp3FileCommand;
	QString mMediaPath;

	trikKernel::Configurer mConfigurer;
};

}
