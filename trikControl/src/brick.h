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
#include <QtCore/QSharedPointer>
#include <memory>

#include <trikKernel/configurer.h>
#include <trikKernel/differentOwnerPointer.h>
#include <functional>
#include "boardImu.h"
#include "deviceFactory.h"
#include "internalBrickInterface.h"
#include "translation.h"

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
class PeripheryCommunicatorInterface;
class CommunicatorProvider;
class PeripheryDriverInterface;
class Keys;
class Led;
class ModuleLoader;
class SoundSensor;
class PowerMotor;
class PwmCapture;
class RangeSensor;
class ServoMotorBase;
class TonePlayer;
class VectorSensor;
class CameraDeviceInterface;
class I2cCommunicator;
class Lidar;
class IrCameraInterface;
class VideoSensorManager;
class CameraManager;
class VideoPreviewSink;

/// Class representing TRIK controller board and devices installed on it, also provides access
/// to peripherals like motors and sensors.
/// Is NOT thread-safe.
class /* TRIKCONTROL_EXPORT for ScriptRunner/PythonQt */ TRIKCONTROL_EXPORT Brick : public InternalBrickInterface
{
	Q_OBJECT

public:
	/// Constructor.
	/// @param systemConfig - file name (with path) of system config, absolute or relative to current directory.
	/// @param modelConfig - file name (with path) of model config, absolute or relative to current directory.
	/// @param mediaPath - path to the directory with media files (it is expected to be ending with "/").
	Brick(const QString &systemConfig, const QString &modelConfig, const QString &mediaPath,
		const std::shared_ptr<VideoPreviewSink> &previewSink);

	/// Secondary constructor, takes explicit hardware abstraction object.
	/// @param hardwareAbstraction - hardware abstraction layer implementation.
	/// @param systemConfig - file name (with path) of system config, absolute or relative to current directory.
	/// @param modelConfig - file name (with path) of model config, absolute or relative to current directory.
	/// @param mediaPath - path to the directory with media files (it is expected to be ending with "/").
	Brick(trikHal::HardwareAbstractionInterface &hardwareAbstraction, const QString &systemConfig,
		const QString &modelConfig, const QString &mediaPath,
		const std::shared_ptr<VideoPreviewSink> &previewSink);

	~Brick() override;

	void reset() override;

	trikControl::DisplayWidgetInterface *graphicsWidget() override;

	QString configVersion() const override;

public Q_SLOTS:
	void configure(const QString &portName, const QString &deviceName) override;

	void playSound(const QString &soundFileName) override;

	void playTone(int hzFreq, int msDuration) override;

	void say(const QString &text) override;

	void stop() override;

	// NOLINTNEXTLINE(google-default-arguments)
	void startVideoTranslation(const QString &port, const QVariant &params = QVariant()) override;

	void stopVideoTranslation(const QString &port) override;

	MotorInterface *motor(const QString &port) override;

	PwmCaptureInterface *pwmCapture(const QString &port) override;

	SensorInterface *sensor(const QString &port) override;

	LidarInterface *lidar() override;

	QStringList motorPorts(MotorInterface::Type type) const override;

	QStringList pwmCapturePorts() const override;

	QStringList sensorPorts(SensorInterface::Type type) const override;

	QStringList encoderPorts() const override;

	VectorSensorInterface *accelerometer() override;

	GyroSensorInterface *gyroscope() override;

	LineSensorInterface *lineSensor(const QString &port) override;

	ColorSensorInterface *colorSensor(const QString &port) override;

	ObjectSensorInterface *objectSensor(const QString &port) override;

	I2cDeviceInterface *i2c(int bus, int address, int regSize) override;

	I2cDeviceInterface *smBusI2c(int bus, int address) override;

	QVector<uint8_t> getStillImage(const QString &port) override;

	SoundSensorInterface *soundSensor(const QString &port) override;

	EncoderInterface *encoder(const QString &port) override;

	BatteryInterface *battery() override;

	KeysInterface *keys() override;

	DisplayInterface *display() override;

	LedInterface *led() override;

	GamepadInterface *gamepad() override;

	FifoInterface *fifo(const QString &port) override;

	MarkerInterface *marker() override;

	IrCameraInterface *irCamera() override;

	EventDeviceInterface *eventDevice(const QString &deviceFile) override;

	void stopEventDevice(const QString &deviceFile) override;

private:
	Brick(const trikKernel::DifferentOwnerPointer<trikHal::HardwareAbstractionInterface> &hardwareAbstraction,
		const QString &systemConfig, const QString &modelConfig, const QString &mediaPath,
		const std::shared_ptr<VideoPreviewSink> &previewSink);

	/// Stops the translation on @p port. If @p keepCamera is true the camera is
	/// only streamed off (kept acquired) so a subsequent sensor init can switch
	/// the DSP algorithm without a slow camera reopen; otherwise it is released.
	void stopVideoTranslationInternal(const QString &port, bool keepCamera);

	/// Stops any mjpg-streamer daemons left over from a previous (crashed) run,
	/// so a fresh Brick starts clean instead of leaving orphaned streamers.
	void stopOrphanedStreamers();

	/// Deinitializes and properly shuts down device on a given port.
	void shutdownDevice(const QString &port);

	/// Creates and configures a device on a given port.
	void createDevice(const QString &port);

	/// Registers device creators keyed by configured device class name.
	void registerDeviceFactories();

	I2cDeviceInterface *createI2cDevice(int bus, int address,
		const std::function<trikHal::PeripheryI2cInterface *(void)> &factory);

	/// Hardware absraction object that is used to provide communication with real robot hardware or to simulate it.
	/// Has or hasn't ownership depending on whether it was created by Brick itself or passed from outside.
	trikKernel::DifferentOwnerPointer<trikHal::HardwareAbstractionInterface> mHardwareAbstraction;

	QScopedPointer<PeripheryCommunicatorInterface> mPeripheryCommunicator;
	QScopedPointer<CommunicatorProvider> mCommunicatorProvider;
	PeripheryDriverInterface *mPeripheryDriver = nullptr;
	QScopedPointer<ModuleLoader> mModuleLoader;
	DeviceFactory mDeviceFactory;

	QScopedPointer<VectorSensor> mAccelerometer;
	QScopedPointer<GyroSensor> mGyroscope;
	QScopedPointer<BoardImu> mBoardImu;
	QScopedPointer<Battery> mBattery;
	QScopedPointer<Keys> mKeys;
	QScopedPointer<Display> mDisplay;
	QScopedPointer<Led> mLed;
	QScopedPointer<Gamepad> mGamepad;
	QScopedPointer<TonePlayer> mTonePlayer;
	QHash<QString, CameraDeviceInterface *> mCameras; // Has ownership, keyed by port.
	QScopedPointer<IrCameraInterface> mIrCamera;
	std::shared_ptr<VideoPreviewSink> mPreviewSink;
	QScopedPointer<VideoSensorManager> mVideoSensorManager;
	QSharedPointer<CameraManager> mCameraManager;
	QHash<QString, ServoMotorBase *> mServoMotors; // Has ownership.
	QHash<QString, PwmCapture *> mPwmCaptures; // Has ownership.
	QHash<QString, PowerMotor *> mPowerMotors; // Has ownership.
	QHash<QString, AnalogSensor *> mAnalogSensors; // Has ownership.
	QHash<QString, Encoder *> mEncoders; // Has ownership.
	QHash<QString, DigitalSensor *> mDigitalSensors; // Has ownership.
	QHash<QString, RangeSensor *> mRangeSensors; // Has ownership.
	QHash<QString, SoundSensor *> mSoundSensors; // Has ownership.
	QHash<QString, Lidar *> mLidars; // Has ownership.
	QHash<QString, Fifo *> mFifos; // Has ownership.
	QHash<QString, EventDeviceInterface *> mEventDevices; // Has ownership.
	QHash<uint16_t, I2cDeviceInterface *> mI2cDevices; // Has ownership.

	QString mPlayWavFileCommand;
	QString mPlayMp3FileCommand;
	QString mMediaPath;

	/// Active translations keyed by port.
	QHash<QString, Translation> mTranslations;

	trikKernel::Configurer mConfigurer;
};

}
