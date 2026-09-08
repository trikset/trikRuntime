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

#include "brick.h"

#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
#	include <QtGui/QApplication>
#else
#	include <QtGui/QGuiApplication>
#endif

#include <QtCore/QFileInfo>
#include <QtMultimedia/QCamera>
#include <QtMultimedia/QCameraImageCapture>
#include <QtMultimedia/QCameraInfo>

#include <trikHal/hardwareAbstractionInterface.h>
#include <trikHal/hardwareAbstractionFactory.h>
#include <trikKernel/exceptions/malformedConfigException.h>
#include <trikKernel/timeVal.h>

#include "analogSensor.h"
#include "battery.h"
#include "colorSensor.h"
#include "digitalSensor.h"
#include "display.h"
#include "encoder.h"
#include "eventDevice.h"
#include "fifo.h"
#include "gamepad.h"
#include "gyroSensor.h"
#include "keys.h"
#include "led.h"
#include "videoSensorManager.h"
#include "lineSensor.h"
#include "objectSensor.h"
#include "powerMotor.h"
#include "pwmCapture.h"
#include "rangeSensor.h"
#include "servoMotor.h"
#include "soundSensor.h"
#include "tonePlayer.h"
#include "vectorSensor.h"
#include "cameraDeviceInterface.h"
#include "cameraDevice.h"
#include "cameraManager.h"
#include "i2cDevice.h"
#include "mspI2cCommunicator.h"
#include "lidar.h"
#include "irCamera.h"

#include "mspBusAutoDetector.h"
#include "moduleLoader.h"

#include <QsLog.h>

using namespace trikControl;
using namespace trikKernel;
using namespace trikHal;

Brick::Brick(trikHal::HardwareAbstractionInterface &hardwareAbstraction, const QString &systemConfig,
	const QString &modelConfig, const QString &mediaPath)
	: Brick(createDifferentOwnerPointer(hardwareAbstraction), systemConfig, modelConfig, mediaPath)
{
}

Brick::Brick(const QString &systemConfig, const QString &modelConfig, const QString &mediaPath)
	: Brick(createDifferentOwnerPointer(HardwareAbstractionFactory::create()), systemConfig, modelConfig, mediaPath)
{
}

Brick::Brick(const trikKernel::DifferentOwnerPointer<trikHal::HardwareAbstractionInterface> &hardwareAbstraction,
	const QString &systemConfig, const QString &modelConfig, const QString &mediaPath)
	: mHardwareAbstraction(hardwareAbstraction)
	, mTonePlayer(new TonePlayer())
	, mMediaPath(mediaPath)
	, mConfigurer(systemConfig, modelConfig)
{
	const bool hasGui = (qobject_cast<QGuiApplication *>(QCoreApplication::instance()) != nullptr);
	if (hasGui) {
		mDisplay.reset(new Display(mediaPath));
	} else {
		QLOG_INFO() << "Running in no GUI mode";
	}

	for (const QString &initScript : mConfigurer.initScripts()) {
		if (mHardwareAbstraction->systemConsole().system(initScript) != 0) {
			QLOG_ERROR() << "Init script failed";
		}
	}

	// A previous trikGui/trikRun may have crashed without running ~Brick(),
	// leaving detached mjpg-streamer daemons behind. Stop them now, before any
	// new translation starts, so a fresh run begins from a clean state.
	stopOrphanedStreamers();

	mMspCommunicator.reset(MspBusAutoDetector::createCommunicator(mConfigurer, *mHardwareAbstraction));
	mModuleLoader.reset(new ModuleLoader(mHardwareAbstraction->systemConsole()));

	for (const QString &port : mConfigurer.ports()) {
		QLOG_INFO() << "Creating device on port" << port;
		createDevice(port);
	}

	mBattery.reset(new Battery(*mMspCommunicator));

	mKeys.reset(new Keys(mConfigurer, *mHardwareAbstraction));

	mLed.reset(new Led(mConfigurer, *mHardwareAbstraction));

	if (mConfigurer.isEnabled("gamepad")) {
		mGamepad.reset(new Gamepad(mConfigurer, *mHardwareAbstraction));
	}

	mCameraManager.reset(new CameraManager(mConfigurer, *mHardwareAbstraction));

	if (mConfigurer.isEnabled("dspServer")) {
		mVideoSensorManager.reset(new VideoSensorManager(mConfigurer, *mHardwareAbstraction, mCameraManager));

		// Repaint the display whenever a video sensor stops (deinit or not), so
		// its last frame is cleared before the next sensor initializes.
		connect(mVideoSensorManager.data(), &VideoSensorManager::sensorStopped, this, &Brick::stopped);
	}

	// Accelerometer must be created before gyroscope - gyro depends on it for calibration.
	// Cannot rely on createDevice loop: XML may list gyroscope first.
	if (mConfigurer.ports().contains("boardAccelPort")
		&& mConfigurer.deviceClass("boardAccelPort") == "iioDevice") {
		mAccelerometer.reset(
			new VectorSensor("accelerometer", mConfigurer, *mHardwareAbstraction, "boardAccelPort"));
	}

	if (mConfigurer.ports().contains("boardGyroPort") && mConfigurer.deviceClass("boardGyroPort") == "iioDevice") {
		mGyroscope.reset(new GyroSensor("gyroscope", mConfigurer, *mHardwareAbstraction, mAccelerometer.data(),
			"boardGyroPort"));
	}

	mPlayWavFileCommand = mConfigurer.attributeByDevice("playWavFile", "command");
	mPlayMp3FileCommand = mConfigurer.attributeByDevice("playMp3File", "command");
}

Brick::~Brick()
{
	// Stop every translation, detached ones included: the brick is going away
	// for good, so nothing should outlive it. The streamer processes are
	// stopped explicitly here; the DSP encoders and cameras are torn down by
	// mVideoSensorManager.reset() below.
	for (auto &&port : mTranslations.keys()) {
		const auto &t = mTranslations.value(port);
		if (t.streamerProcess) {
			t.streamerProcess->stop();
		}
	}
	mTranslations.clear();

	qDeleteAll(mServoMotors);
	qDeleteAll(mPwmCaptures);
	qDeleteAll(mPowerMotors);
	qDeleteAll(mEncoders);
	qDeleteAll(mAnalogSensors);
	qDeleteAll(mDigitalSensors);
	qDeleteAll(mRangeSensors);
	qDeleteAll(mSoundSensors);
	qDeleteAll(mFifos);
	qDeleteAll(mEventDevices);
	qDeleteAll(mI2cDevices);
	qDeleteAll(mLidars);
	qDeleteAll(mCameras);

	// Clean up devices before killing hardware abstraction since their finalization may depend on it.
	mMspCommunicator.reset();
	mModuleLoader.reset();

	mAccelerometer.reset();
	mGyroscope.reset();
	mBattery.reset();
	mKeys.reset();
	mDisplay.reset();
	mLed.reset();
	mGamepad.reset();
	mIrCamera.reset();
	mVideoSensorManager.reset();
}

DisplayWidgetInterface *Brick::graphicsWidget()
{
	if (mDisplay) {
		return &mDisplay->graphicsWidget();
	} else {
		return nullptr;
	}
}

QString Brick::configVersion() const
{
	return mConfigurer.version();
}

void Brick::configure(const QString &portName, const QString &deviceName)
{
	// A translation (detached or not) streaming on @p port must be stopped
	// before the port is reconfigured. Keep the camera acquired so the manager
	// can just switch the DSP algorithm when the new sensor is initialized.
	if (mTranslations.contains(portName)) {
		stopVideoTranslationInternal(portName, /*keepCamera=*/true);
	}

	// This method is kept for backward compatibility with the generated
	// (PythonQt/JS) bindings: they may call configure() with a video sensor
	// class (lineSensor, objectSensor, colorSensor) or a still camera on a
	// video port.
	//
	// Video ports (video1, video2, usb-camera, ...) host
	// camera-based devices that all share the same physical camera through
	// VideoSensorManager/CameraManager. There is no standalone "device" to
	// shut down on such a port - the camera itself is reused, so at most the
	// high-level object (a sensor or a camera) has to be (re)created.
	const auto isVideoSensor = VideoSensorManager::isVideoSensor(deviceName);
	const auto isVideoPort =
		portName.startsWith(QStringLiteral("video")) || portName.startsWith(QStringLiteral("usb-camera"));

	if (isVideoPort
		&& (isVideoSensor || deviceName == QStringLiteral("photo") || deviceName == QStringLiteral("camera"))) {
		// A video sensor just asks VideoSensorManager to (re)create the sensor.
		if (isVideoSensor && mVideoSensorManager) {
			mVideoSensorManager->create(portName, deviceName);
		}
		// A still camera is created lazily in getStillImage(port), so there is
		// nothing to configure or shut down here.
		return;
	}

	// Regular device: shut the old one down, record the new device type in the
	// config and instantiate it.
	shutdownDevice(portName);
	mConfigurer.configure(portName, deviceName);
	createDevice(portName);
}

void Brick::reset()
{
	stop();
	mKeys->reset();
	if (mDisplay) {
		mDisplay->reset();
	}

	if (mGamepad) {
		mGamepad->reset();
	}

	/// @todo Temporary, we need more carefully init/deinit range sensors.
	for (auto &&rangeSensor : mRangeSensors) {
		rangeSensor->init();
	}

	Q_EMIT resetCompleted();
}

void Brick::startVideoTranslation(const QString &port, const QVariant &params) // NOLINT(google-default-arguments)
{
	QLOG_INFO() << "Brick::startVideoTranslation: port" << port;

	const auto &paramsMap = params.toMap();
	const auto isUsb = port.startsWith(QStringLiteral("usb-camera"));

	if (!isUsb && !port.startsWith(QStringLiteral("video"))) {
		QLOG_ERROR() << "Brick::startVideoTranslation: unsupported port" << port
			     << "(supported ports: video* or usb-camera)";
		return;
	}

	// The DSP is single-channel: only one ov7670 translation can run at a time.
	// A new video translation supersedes any other active video translation
	// (even a detached one) - otherwise the old encoder would be silently
	// evicted from the DSP while its streamer keeps serving a stale FIFO.
	if (!isUsb) {
		QStringList superseded;
		for (auto it = mTranslations.constBegin(); it != mTranslations.constEnd(); ++it) {
			if (!it.value().isUsb && it.key() != port) {
				superseded << it.key();
			}
		}
		for (auto &&other : superseded) {
			stopVideoTranslationInternal(other, /*keepCamera=*/false);
		}
	}

	// A re-start on an already translated port replaces the old translation.
	if (mTranslations.contains(port)) {
		stopVideoTranslationInternal(port, false);
	}

	// Kick any other client off @p port first: stop the video sensor (if any)
	// so the translation can take exclusive ownership of the camera.
	if (mVideoSensorManager) {
		mVideoSensorManager->releasePort(port);
	}

	const auto detached = paramsMap.value(QStringLiteral("detached"), false).toBool();

	if (isUsb) {
		// USB webcam: mjpg-streamer opens the device directly over UVC, so the
		// device must be released (no DSP encoder involved).
		if (mCameraManager) {
			mCameraManager->close(port);
		}
	} else {
		// ov7670 camera port: schedule the DSP JPEG encoder. Its result is
		// streamed into the FIFO that mjpg-streamer's input_fifo.so reads.
		if (mVideoSensorManager) {
			if (auto &&encoder = mVideoSensorManager->jpegEncoderSensor(port)) {
				const auto jpegQuality = paramsMap.value(QStringLiteral("jpeg-qual"), 30).toInt();
				const auto whiteBlack = paramsMap.value(QStringLiteral("white-black"), false).toBool();
				encoder->init(static_cast<uint8_t>(jpegQuality), whiteBlack, false);
			}
		}
	}

	// A detached translation must survive stop()/configure() rescheduling.
	if (mVideoSensorManager) {
		mVideoSensorManager->setPortDetached(port, detached);
	}

	// Run the mjpg-streamer launcher script ("start <port> [device]"). Its path
	// is recorded in the system config per videoDevice port (see CameraManager).
	const auto &script = mCameraManager ? mCameraManager->streamerScript(port) : QString();

	if (script.isEmpty()) {
		QLOG_ERROR() << "Brick::startVideoTranslation: no mjpg-streamer script configured for port" << port;
		return;
	}

	const QString device = mCameraManager ? mCameraManager->deviceFile(port) : QString();

	// The streamer is launched as a QProcess owned by a StreamerProcess RAII
	// wrapper (not via system() + the script's own "&" backgrounding), so Brick
	// can wait for it to actually terminate - and thus release the camera - on
	// stop. No QObject parent: start/stop may run on the script thread while
	// Brick lives on the GUI thread.
	auto streamer = QSharedPointer<Translation::StreamerProcess>::create();

	QLOG_INFO() << "Brick::startVideoTranslation: launching mjpg-streamer port=" << port << "isUsb=" << isUsb
		    << "detached=" << detached << "script=" << script;

	if (!streamer->start(script, port, device)) {
		QLOG_ERROR() << "Failed to start mjpg-streamer for port" << port;
		return;
	}

	mTranslations.insert(port, {script, detached, isUsb, streamer});
}

void Brick::stopVideoTranslation(const QString &port)
{
	if (!port.startsWith(QStringLiteral("usb-camera")) && !port.startsWith(QStringLiteral("video"))) {
		QLOG_ERROR() << "Brick::stopVideoTranslation: unsupported port" << port
			     << "(supported ports: video* or usb-camera)";
		return;
	}

	stopVideoTranslationInternal(port, false);
}

void Brick::stopVideoTranslationInternal(const QString &port, bool keepCamera)
{
	auto it = mTranslations.find(port);
	if (it == mTranslations.end()) {
		QLOG_INFO() << "Brick::stopVideoTranslation: no translation on port" << port;
		return;
	}

	const auto &translation = it.value();

	QLOG_INFO() << "Brick::stopVideoTranslation: stopping mjpg-streamer port=" << port
		    << "isUsb=" << translation.isUsb << "detached=" << translation.detached
		    << "script=" << translation.streamerScript;

	// Symmetric to the legacy codegen
	// (mjpg-streamer stop && mjpg-encoder stop): stop the streamer process
	// first, then the DSP encoder feeding it. The process is owned by Brick, so
	// stop() blocks until it exits and releases the camera.
	if (translation.streamerProcess) {
		translation.streamerProcess->stop();
	}

	if (mVideoSensorManager) {
		mVideoSensorManager->stopTranslation(port, keepCamera);
	}

	mTranslations.erase(it);
}

void Brick::stopOrphanedStreamers()
{
	// Every videoDevice port records its mjpg-streamer launcher script in the
	// config. Running "stop" per port is idempotent: with no daemon it is a
	// no-op, so on a clean start this loop does nothing. It only matters after
	// a crash, where ~Brick() never ran and the daemons are still alive.
	for (auto &&port : mConfigurer.ports()) {
		if (mConfigurer.deviceClass(port) != QStringLiteral("videoDevice")) {
			continue;
		}

		QString defaultScript;
		const auto &script = mConfigurer.attributeByPort(port, "mjpgStreamerScript", &defaultScript);
		if (script.isEmpty()) {
			continue;
		}

		if (mHardwareAbstraction->systemConsole().system(script + " stop " + port) != 0) {
			QLOG_INFO() << "Brick: no orphaned mjpg-streamer on port" << port;
		}
	}
}

void Brick::playSound(const QString &soundFileName)
{
	QLOG_INFO() << "Playing " << soundFileName;

	QFileInfo fileInfo(soundFileName);

	if (!fileInfo.exists()) {
		fileInfo = QFileInfo(mMediaPath + soundFileName);
	}

	QString command;

	if (fileInfo.suffix() == "wav") {
		command = mPlayWavFileCommand.arg(fileInfo.absoluteFilePath());
	} else if (fileInfo.suffix() == "mp3") {
		command = mPlayMp3FileCommand.arg(fileInfo.absoluteFilePath());
	}

	if (command.isEmpty() || mHardwareAbstraction->systemConsole().system(command) != 0) {
		QLOG_ERROR() << "Play sound failed";
	}
}

void Brick::playTone(int hzFreq, int msDuration)
{
	QLOG_INFO() << "Playing tone (" << hzFreq << "," << msDuration << ")";

	if (hzFreq < 0 || msDuration < 0) {
		return;
	}

	// mHardwareAbstraction->systemSound()->playTone(hzFreq, msDuration);
	// mTonePlayer->play(hzFreq, msDuration);
	QMetaObject::invokeMethod(mTonePlayer.data(),
		[this, hzFreq, msDuration]() { mTonePlayer->play(hzFreq, msDuration); });
}

void Brick::say(const QString &text)
{
	QStringList args {"-c", "exec /etc/trik/say  \"" + text + "\""};
	mHardwareAbstraction->systemConsole().startProcess("sh", args);
}

void Brick::stop()
{
	QLOG_INFO() << "Stopping brick";

	mTonePlayer->stop();

	for (auto &&servoMotor : mServoMotors) {
		servoMotor->powerOff();
	}

	for (auto &&powerMotor : mPowerMotors) {
		powerMotor->powerOff();
	}

	if (mDisplay) {
		mDisplay->hide();
	}

	// Stop non-detached translations. Detached ones keep their DSP encoder,
	// camera and streamer alive until stopVideoTranslation() or the destructor.
	QStringList detachedPorts;
	{
		QStringList toStop;
		for (auto it = mTranslations.constBegin(); it != mTranslations.constEnd(); ++it) {
			if (it.value().detached) {
				detachedPorts << it.key();
			} else {
				toStop << it.key();
			}
		}
		// Only the streamer process is stopped here; the DSP encoders and the
		// cameras are torn down by VideoSensorManager::stop()/clear() below.
		for (auto &&port : toStop) {
			auto &translation = mTranslations[port];
			if (translation.streamerProcess) {
				translation.streamerProcess->stop();
			}
			mTranslations.remove(port);
		}
	}

	if (mVideoSensorManager) {
		QLOG_INFO() << "Brick::stop: stopping video sensor manager";
		mVideoSensorManager->stop(); // skips detached ports
		mVideoSensorManager->clear(); // skips detached sensors
	}

	// Force-release any leaked camera, unless a detached translation must keep
	// its device open.
	if (mCameraManager && detachedPorts.isEmpty()) {
		QLOG_INFO() << "Brick::stop: closing camera manager";
		mCameraManager->close();
	}

	// All video sensors (and the cameras backing them) are now stopped and the
	// framebuffer is closed. Notify the GUI so it can repaint the screen and
	// clear any leftover video frames.
	Q_EMIT stopped();
	QLOG_INFO() << "Brick::stop: emitted stopped()";

	for (auto &&soundSensor : mSoundSensors) {
		if (soundSensor->status() == DeviceInterface::Status::ready) {
			soundSensor->stop();
		}
	}

	for (auto &&rangeSensor : mRangeSensors) {
		rangeSensor->stop();
	}

	if (mIrCamera) {
		mIrCamera->stop();
	}

	qDeleteAll(mEventDevices);
	mEventDevices.clear();
}

MotorInterface *Brick::motor(const QString &port)
{
	if (mPowerMotors.contains(port)) {
		return mPowerMotors[port];
	} else if (mServoMotors.contains(port)) {
		return mServoMotors[port];
	} else {
		return nullptr;
	}
}

PwmCaptureInterface *Brick::pwmCapture(const QString &port)
{
	return mPwmCaptures.value(port, nullptr);
}

SensorInterface *Brick::sensor(const QString &port)
{
	if (mAnalogSensors.contains(port)) {
		return mAnalogSensors[port];
	} else if (mDigitalSensors.contains(port)) {
		return mDigitalSensors[port];
	} else if (mRangeSensors.contains(port)) {
		return mRangeSensors[port];
	} else {
		return nullptr;
	}
}

LidarInterface *Brick::lidar()
{
	auto port = "lidarPort";
	if (mLidars.contains(port)) {
		return mLidars[port];
	} else {
		return nullptr;
	}
}

QStringList Brick::motorPorts(MotorInterface::Type type) const
{
	switch (type) {
	case MotorInterface::Type::powerMotor: {
		return mPowerMotors.keys();
	}
	case MotorInterface::Type::servoMotor: {
		return mServoMotors.keys();
	}
	}

	return {};
}

QStringList Brick::pwmCapturePorts() const
{
	return mPwmCaptures.keys();
}

QStringList Brick::sensorPorts(SensorInterface::Type type) const
{
	switch (type) {
	case SensorInterface::Type::analogSensor: {
		return mAnalogSensors.keys();
	}
	case SensorInterface::Type::digitalSensor: {
		return mDigitalSensors.keys() + mRangeSensors.keys();
	}
	case SensorInterface::Type::specialSensor: {
		// Special sensors can not be connected to standard ports, they have their own methods to access them.
		return {};
	}
	}

	return {};
}

EncoderInterface *Brick::encoder(const QString &port)
{
	return mEncoders.value(port, nullptr);
}

BatteryInterface *Brick::battery()
{
	return mBattery.data();
}

VectorSensorInterface *Brick::accelerometer()
{
	return mAccelerometer.data();
}

GyroSensorInterface *Brick::gyroscope()
{
	return mGyroscope.data();
}

LineSensorInterface *Brick::lineSensor(const QString &port)
{
	return mVideoSensorManager ? mVideoSensorManager->lineSensor(port) : nullptr;
}

ColorSensorInterface *Brick::colorSensor(const QString &port)
{
	return mVideoSensorManager ? mVideoSensorManager->colorSensor(port) : nullptr;
}

ObjectSensorInterface *Brick::objectSensor(const QString &port)
{
	return mVideoSensorManager ? mVideoSensorManager->objectSensor(port) : nullptr;
}

I2cDeviceInterface *Brick::createI2cDevice(int bus, int address,
	const std::function<trikHal::MspI2cInterface *()> &factory)
{
	uint8_t _bus = bus & 0xFF;
	uint8_t _address = address & 0xFF;
	uint16_t mhash = (_bus << 8) | _address;
	if (mI2cDevices.contains(mhash)) {
		return mI2cDevices[mhash];
	} else {
		auto i2cDeviceUnique = std::make_unique<I2cDevice>(mConfigurer, factory(), _bus, _address);

		if (i2cDeviceUnique->status() == DeviceInterface::Status::permanentFailure) {
			QLOG_ERROR() << "Could not open device on bus" << bus << "and address " << address;
			return nullptr;
		}

		auto *i2cDevice = i2cDeviceUnique.release();
		mI2cDevices.insert(mhash, i2cDevice);
		return i2cDevice;
	}
}

I2cDeviceInterface *Brick::i2c(int bus, int address, int regSize)
{
	if (regSize != 1 && regSize != 2) {
		return nullptr;
	}

	return createI2cDevice(bus, address,
		[this, regSize]() { return mHardwareAbstraction->createCommonI2c(regSize); });
}

I2cDeviceInterface *Brick::smBusI2c(int bus, int address)
{
	return createI2cDevice(bus, address, [this]() { return mHardwareAbstraction->createMspI2c(); });
}

QVector<uint8_t> Brick::getStillImage(const QString &port)
{
	if (!mCameraManager) {
		mCameraManager.reset(new CameraManager(mConfigurer, *mHardwareAbstraction));
	}

	// Create the CameraDevice for @p port lazily and cache it. Its state is
	// self-contained, so the device can be reused for subsequent photos.
	auto *camera = mCameras.value(port, nullptr);
	if (!camera) {
		camera = new CameraDevice(port, mMediaPath, mConfigurer, mCameraManager);
		mCameras.insert(port, camera);
	}

	return camera->getPhoto();
}

SoundSensorInterface *Brick::soundSensor(const QString &port)
{
	return mSoundSensors.contains(port) ? mSoundSensors[port] : nullptr;
}

KeysInterface *Brick::keys()
{
	return mKeys.data();
}

QStringList Brick::encoderPorts() const
{
	return mEncoders.keys();
}

DisplayInterface *Brick::display()
{
	return mDisplay.data();
}

LedInterface *Brick::led()
{
	return mLed.data();
}

GamepadInterface *Brick::gamepad()
{
	return mGamepad.data();
}

trikControl::FifoInterface *Brick::fifo(const QString &port)
{
	return mFifos[port];
}

MarkerInterface *Brick::marker()
{
	return nullptr;
}

IrCameraInterface *Brick::irCamera()
{
	return mIrCamera.data();
}

EventDeviceInterface *Brick::eventDevice(const QString &deviceFile)
{
	if (!mEventDevices.contains(deviceFile)) {
		EventDeviceInterface *const eventDevice = new EventDevice(deviceFile, *mHardwareAbstraction);
		if (eventDevice->status() != EventDeviceInterface::Status::permanentFailure) {
			mEventDevices.insert(deviceFile, eventDevice);
		}
	}

	return mEventDevices[deviceFile];
}

void Brick::stopEventDevice(const QString &deviceFile)
{
	if (mEventDevices.contains(deviceFile)) {
		mEventDevices[deviceFile]->deleteLater();
		mEventDevices.remove(deviceFile);
	}
}

void Brick::shutdownDevice(const QString &port)
{
	const QString &deviceClass = mConfigurer.deviceClass(port);
	if (deviceClass == "servoMotor") {
		mServoMotors[port]->powerOff();
		delete mServoMotors[port];
		mServoMotors.remove(port);
	} else if (deviceClass == "pwmCapture") {
		delete mPwmCaptures[port];
		mPwmCaptures.remove(port);
	} else if (deviceClass == "powerMotor") {
		mPowerMotors[port]->powerOff();
		delete mPowerMotors[port];
		mPowerMotors.remove(port);
	} else if (deviceClass == "analogSensor") {
		delete mAnalogSensors[port];
		mAnalogSensors.remove(port);
	} else if (deviceClass == "digitalSensor") {
		delete mDigitalSensors[port];
		mDigitalSensors.remove(port);
	} else if (deviceClass == "rangeSensor") {
		mRangeSensors[port]->stop();
		delete mRangeSensors[port];
		mRangeSensors.remove(port);
	} else if (deviceClass == "encoder") {
		delete mEncoders[port];
		mEncoders.remove(port);
	} else if (deviceClass == "fifo") {
		delete mFifos[port];
		mFifos.remove(port);
	} else if (deviceClass == "lidar") {
		delete mLidars[port];
		mLidars.remove(port);
	}
}

void Brick::createDevice(const QString &port)
{
	try {
		const QString &deviceClass = mConfigurer.deviceClass(port);
		if (deviceClass == "servoMotor") {
			mServoMotors.insert(port, new ServoMotor(port, mConfigurer, *mHardwareAbstraction));
		} else if (deviceClass == "pwmCapture") {
			mPwmCaptures.insert(port, new PwmCapture(port, mConfigurer, *mHardwareAbstraction));
		} else if (deviceClass == "powerMotor") {
			mPowerMotors.insert(port, new PowerMotor(port, mConfigurer, *mMspCommunicator));
		} else if (deviceClass == "analogSensor") {
			mAnalogSensors.insert(port, new AnalogSensor(port, mConfigurer, *mMspCommunicator));
		} else if (deviceClass == "digitalSensor") {
			mDigitalSensors.insert(port, new DigitalSensor(port, mConfigurer, *mHardwareAbstraction));
		} else if (deviceClass == "rangeSensor") {
			mRangeSensors.insert(port,
				new RangeSensor(port, mConfigurer, *mModuleLoader, *mHardwareAbstraction));

			/// @todo Range sensor shall be turned on only when needed.
			mRangeSensors[port]->init();
		} else if (deviceClass == "encoder") {
			mEncoders.insert(port, new Encoder(port, mConfigurer, *mMspCommunicator));
		} else if (deviceClass == "soundSensor") {
			mSoundSensors.insert(port, new SoundSensor(port, mConfigurer, *mHardwareAbstraction));

			/// @todo This will work only in case when there can be only one sound sensor launched at a time.
		} else if (deviceClass == "fifo") {
			mFifos.insert(port, new Fifo(port, mConfigurer, *mHardwareAbstraction));
		} else if (deviceClass == "lidar") {
			mLidars.insert(port, new Lidar(port, mConfigurer, *mHardwareAbstraction));
		} else if (deviceClass == "irCamera") {
			QScopedPointer<IrCameraInterface> tmp(new IrCamera(port, mConfigurer, *mHardwareAbstraction));
			mIrCamera.swap(tmp);
		}
	} catch (MalformedConfigException &e) {
		QLOG_ERROR() << "Config for port" << port << "is malformed:" << e.errorMessage();
		QLOG_ERROR() << "Ignoring device";
	}
}
